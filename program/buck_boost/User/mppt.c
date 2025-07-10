//
// Created by »Æ on 2025/7/1.
//

#include "mppt.h"
#include "main.h"
#include "hrtim.h"
#include "pid.h"

#define  Interruption_frequency 50000
#define  voltage_coefficient (33.0f/4095)
#define Current_coefficient (3.0f/3276)
#define Io_Current_coefficient (6.0f/3276)
#define power_coefficient  (voltage_coefficient*Current_coefficient)

unsigned int run_time = 0;
unsigned int Vin_adc = 0;
unsigned int Iin_adc = 0;
unsigned int Vo_adc = 0;
unsigned int Io_adc = 0;
float D = 0.0f;

#define P (0.000001f)
#define I (0.0000001f)

control_mode running_state = buck_boost;
mppt_struction mppt = {(int) (3.0f / voltage_coefficient), (int) (0.3f / voltage_coefficient), 0, 0,
                       0, 0, 0, 0, 0, 0, 0};

void get_open_circuit_voltage(mppt_struction *mppt_struct) {
    static uint16_t count = 0;
    static unsigned int ad_sum = 0;
    static unsigned char flag = 0;
    static unsigned char data_count = 0;

    if (Iout < 100 && VIN > 1480) {
        count++;
        if (count == 1000 && flag == 0) {
            count = 0;
            flag = 1;
        } else if (flag == 1) {
            data_count++;
            ad_sum = ad_sum + VIN;
            if (data_count == 100) {
                data_count = 0;
                mppt_struct->mppt_open_circuit_voltage = (float) ad_sum / 100.0f;
                mppt_struct->get_mppt_open_circuit_voltage_flag = 1;
                flag = 0;
                ad_sum = 0;
            }

        }
    }

}


void mppt_buck_mode(float duty) {
    static HRTIM_TimeBaseCfgTypeDef buck_config;
    static uint16_t duty_int = 0;
    buck_config.Period = 54400;
    buck_config.RepetitionCounter = 0;
    buck_config.PrescalerRatio = HRTIM_PRESCALERRATIO_DIV1;
    buck_config.Mode = HRTIM_MODE_CONTINUOUS;
    HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &buck_config);
    if (duty >= 0.9f) {
        duty = 0.9f;
    }
    duty_int = (uint16_t) (duty * 54400);
    hhrtim1.Instance->sTimerxRegs[3].CMP1xR = 1000;
    hhrtim1.Instance->sTimerxRegs[5].CMP1xR = duty_int;
}

void mppt_buck_boost_mode(float duty) {
    static HRTIM_TimeBaseCfgTypeDef buck_config;
    static uint16_t duty_int = 0;
    buck_config.Period = 54400;
    buck_config.RepetitionCounter = 0;
    buck_config.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL16;
    buck_config.Mode = HRTIM_MODE_CONTINUOUS;
    HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &buck_config);
    if (duty >= 0.9f) {
        duty = 0.9f;
    }
    duty_int = (uint16_t) (duty * 54400);
    hhrtim1.Instance->sTimerxRegs[3].CMP1xR = duty_int;
    hhrtim1.Instance->sTimerxRegs[5].CMP1xR = duty_int;
}


void protect_task(void) {
    static uint8_t error_count = 0;
    static uint32_t all_safe_count = 0;
    if (VIN * voltage_coefficient < 0.0f || VOUT * voltage_coefficient > 30.0f ||
        Iout * Iout_ADC_coefficient > 5.0f) {
        error_count++;
    } else {
        all_safe_count++;
    }

    if (error_count == 20) {
        error_count = 0;
        protect_flag = 1;
    } else if (all_safe_count == 100000) {
        all_safe_count = 0;
        protect_flag = 0;
    }
    if (protect_flag == 1) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
    } else if (protect_flag == 0) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
    }
}

void mppt_Perturb_and_Observe(mppt_struction *mppt_struct) {
    mppt_struct->power = VIN * Iin;
    mppt_struct->vin = VIN;
    if (mppt_struct->power > mppt_struct->last_power) {
        if (mppt_struct->vin > mppt_struct->vin_last) {
            mppt_struct->mppt_vin_target = mppt_struct->mppt_vin_target + mppt_struct->mppt_deltaV;
        } else if (mppt_struct->vin < mppt_struct->vin_last) {
            mppt_struct->mppt_vin_target = mppt_struct->mppt_vin_target - mppt_struct->mppt_deltaV;
        }
    } else if (mppt_struct->power < mppt_struct->last_power) {
        if (mppt_struct->vin < mppt_struct->vin_last) {
            mppt_struct->mppt_vin_target = mppt_struct->mppt_vin_target + mppt_struct->mppt_deltaV;
        } else if (mppt_struct->vin > mppt_struct->vin_last) {
            mppt_struct->mppt_vin_target = mppt_struct->mppt_vin_target - mppt_struct->mppt_deltaV;
        }
    }
    mppt_struct->last_power = mppt_struct->power;
    mppt_struct->vin_last = mppt_struct->vin;

}


void
mppt_Constant_voltage_method(mppt_struction *mppt_struct, const unsigned int *Vo_ADC_value,
                             const unsigned int *I_ADC_value,
                             const unsigned int *vin_ADC_value) {

    mppt_struct->power = VIN * Iin;
    mppt_struct->vin = VIN;
    mppt_struct->mppt_vin_target = (int) (0.78f * mppt_struct->mppt_open_circuit_voltage);
    if (running_state == buck_boost) {
        mppt_struct->mppt_vin_error = (int) VIN - mppt_struct->mppt_vin_target;
        mppt_struct->mppt_contral_output =
                P * (float) mppt_struct->mppt_vin_error + I * (float) mppt_struct->mppt_vin_error_integral;
        mppt_struct->mppt_vin_error_integral = mppt_struct->mppt_vin_error_integral + mppt_struct->mppt_vin_error;


        if (mppt_struct->mppt_contral_output > 0.62f) {
            mppt_struct->mppt_contral_output = 0.62f;
        }
        if (mppt_struct->mppt_contral_output < 0.1f) {
            mppt_struct->mppt_contral_output = 0.1f;
        }
        mppt_buck_boost_mode(mppt_struct->mppt_contral_output);
    }
    if (running_state == buck) {
        mppt_struct->mppt_vin_error = (int) VIN - mppt_struct->mppt_vin_target;
        mppt_struct->mppt_contral_output =
                P * (float) mppt_struct->mppt_vin_error + I * (float) mppt_struct->mppt_vin_error_integral;
        mppt_struct->mppt_vin_error_integral = mppt_struct->mppt_vin_error_integral + mppt_struct->mppt_vin_error;


        if (mppt_struct->mppt_contral_output > 0.65f) {
            mppt_struct->mppt_contral_output = 0.65f;
        }
        if (mppt_struct->mppt_contral_output < 0.1f) {
            mppt_struct->mppt_contral_output = 0.1f;
        }
        mppt_buck_mode(mppt_struct->mppt_contral_output);
    }


}
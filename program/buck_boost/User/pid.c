//
// Created by »Æ on 2024/6/7.
//

#include "pid.h"


volatile uint16_t AD_Value[4] = {0};
dcdc_control buck_control = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, buck};
dcdc_control buck_boost_control = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, buck_boost};
control_mode running_mode;
volatile uint8_t protect_flag = 0;
uint8_t begin_flag = 0;

void dcdc_control_init(dcdc_control *control, float P, float I, int error_integral_limit, uint16_t feedforward,
                       int target) {
    control->P = P;
    control->I = I;
    control->error_integral_limit = error_integral_limit;
    control->feedforward = feedforward;
    control->control_target = target;
}


void PI_updata(dcdc_control *control) {
    control->error = control->control_target - VOUT;
    control->error_integral = (control->error_integral + control->error);
//    if (control->error_integral > control->error_integral_limit) {
//        control->error_integral = control->error_integral_limit;
//    }
    control->PI_output = (int) ((control->P * (float) control->error) +
                                control->I * (float) control->error_integral);
}


inline void updata_pwm(dcdc_control *control) {
    if (control->mode == buck) {
        control->feedforward = (uint16_t) (54400 * 20.0f / (VIN * VIN_ADC_coefficient));
        if (begin_flag == 1) {

            PI_updata(control);
            control->control_output = (uint16_t) (control->feedforward + (control->PI_output >> 2));
            if (control->control_output > pwm_H_limit) {
                control->control_output = pwm_H_limit;
            } else if (control->control_output < pwm_L_limit) {
                control->control_output = pwm_L_limit;
            }
            hhrtim1.Instance->sTimerxRegs[3].CMP1xR = 1000;
            hhrtim1.Instance->sTimerxRegs[5].CMP1xR = control->control_output;
        } else {
            hhrtim1.Instance->sTimerxRegs[3].CMP1xR = 1000;
            hhrtim1.Instance->sTimerxRegs[5].CMP1xR = control->feedforward;
        }

    } else if (control->mode == buck_boost) {
        control->feedforward = (uint16_t) ((20.0f /
                                            (VIN * VIN_ADC_coefficient + 20.0f)) *
                                           54400);
        if (begin_flag == 1) {
            PI_updata(control);
            control->control_output = (uint16_t) (control->feedforward + (control->PI_output >> 2));
            if (control->control_output > 32000) {
                control->control_output = 32000;
            } else if (control->control_output < pwm_L_limit) {
                control->control_output = pwm_L_limit;
            }
            hhrtim1.Instance->sTimerxRegs[3].CMP1xR = control->control_output;
            hhrtim1.Instance->sTimerxRegs[5].CMP1xR = control->control_output;
        } else {
            hhrtim1.Instance->sTimerxRegs[3].CMP1xR = control->feedforward;
            hhrtim1.Instance->sTimerxRegs[5].CMP1xR = control->feedforward;
        }

    }
}

void buck_on(dcdc_control *control) {
    control->mode = buck;
    static HRTIM_TimeBaseCfgTypeDef buck_config;
    buck_config.Period = 54400;
    buck_config.RepetitionCounter = 0;
    buck_config.PrescalerRatio = HRTIM_PRESCALERRATIO_DIV1;
    buck_config.Mode = HRTIM_MODE_CONTINUOUS;
    HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &buck_config);
}

void buck_boost_on(dcdc_control *control) {
    control->mode = buck_boost;
    static HRTIM_TimeBaseCfgTypeDef buck_config;
    buck_config.Period = 54400;
    buck_config.RepetitionCounter = 0;
    buck_config.PrescalerRatio = HRTIM_PRESCALERRATIO_MUL16;
    buck_config.Mode = HRTIM_MODE_CONTINUOUS;
    HAL_HRTIM_TimeBaseConfig(&hhrtim1, HRTIM_TIMERINDEX_TIMER_D, &buck_config);

}

inline void control_run(dcdc_control *buck_controler, dcdc_control *buck_boost_controler) {
    if (running_mode == buck) {
        updata_pwm(buck_controler);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
    } else if (running_mode == buck_boost) {
        updata_pwm(buck_boost_controler);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
    }
}

void Hysteresis_comparator(dcdc_control *buck_controler, dcdc_control *buck_boost_controler) {

    if (buck_controler->mode == buck && VIN < 2800 - Hysteresis_Band) {
        buck_boost_on(buck_boost_controler);
        running_mode = buck_boost;
    } else if (buck_boost_controler->mode == buck_boost && VIN > 2800 + Hysteresis_Band) {
        buck_on(buck_controler);
        running_mode = buck;
    }
}



//
// Created by »Æ on 2025/7/1.
//

#ifndef PROJECT_MPPT_H
#define PROJECT_MPPT_H

#define  voltage_coefficient (33.0f/4095)
#define Current_coefficient (3.0f/3276)
#define Io_Current_coefficient (6.0f/3276)
#define power_coefficient  (voltage_coefficient*Current_coefficient)

typedef struct {
    int mppt_vin_target;
    int mppt_deltaV;
    unsigned int power;
    unsigned int last_power;
    unsigned int vin;
    unsigned int vin_last;
    int mppt_vin_error;
    int mppt_vin_error_integral;
    float mppt_contral_output;
    float mppt_open_circuit_voltage;
    unsigned char get_mppt_open_circuit_voltage_flag;
} mppt_struction;


extern mppt_struction mppt;

extern void get_open_circuit_voltage(mppt_struction *mppt_struct);

extern void mppt_buck_mode(float duty);

extern void
mppt_Perturb_and_Observe(mppt_struction *mppt_struct, const unsigned int *Vo_ADC_value, const unsigned int *I_ADC_value,
                         const unsigned int *vin_ADC_value);

extern void mppt_buck_boost_mode(float duty);

extern void protect_task(void);

extern void mppt_perturb(mppt_struction *mppt_struct);

#endif //PROJECT_MPPT_H

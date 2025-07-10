//
// Created by »Æ on 2024/6/7.
//

#ifndef PROJECT_PID_H
#define PROJECT_PID_H

#include "main.h"
#include "hrtim.h"

#define pwm_H_limit 50000
#define pwm_L_limit 5440
#define Hysteresis_Band 25
#define VOUT AD_Value[0]
#define Iout AD_Value[2]
#define Iin AD_Value[3]
#define VIN AD_Value[1]
#define VIN_ADC_coefficient (33.0f/4095)
#define VOUT_ADC_coefficient (21.0f/4095.0f)
#define Iout_ADC_coefficient (3.0f/3276)
#define Iin_ADC_coefficient (3.0f/3276)
typedef enum {
    buck,
    buck_boost
} control_mode;

typedef struct {
    float P;
    float I;
    int error;
    int last_err;
    int error_integral;
    int error_integral_limit;
    int PI_output;

    int control_target;

    uint16_t feedforward;
    uint16_t control_output;
    control_mode mode;
} dcdc_control;


extern volatile uint16_t AD_Value[4];
extern dcdc_control buck_control;
extern dcdc_control buck_boost_control;
extern volatile uint8_t protect_flag;

extern void Hysteresis_comparator(dcdc_control *buck_controler, dcdc_control *buck_boost_controler);

extern void dcdc_control_init(dcdc_control *control, float P, float I, int error_integral_limit, uint16_t feedforward,
                              int target);

extern void control_run(dcdc_control *buck_controler, dcdc_control *buck_boost_controler);

extern uint8_t begin_flag;


#endif //PROJECT_PID_H

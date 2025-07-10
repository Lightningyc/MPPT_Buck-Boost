#ifndef _uart_to_printf_H
#define _uart_to_printf_H


#include "main.h"
#include "usart.h"
#include "stdio.h"
#include "pid.h"
#include <string.h>
#include "mppt.h"

#define data_num 24

#define buffer_size (4*data_num+4)

typedef struct vofa_JustFloat {
    float fdata[data_num];
    unsigned char tail[4];
} vofa_JustFloat_t;

extern vofa_JustFloat_t vofa_JustFloat;
extern unsigned char vofa_data[buffer_size];
extern uint32_t task_run;

inline void printf_data(void) {
    task_run++;
    vofa_JustFloat.fdata[0] = VIN * voltage_coefficient;
    vofa_JustFloat.fdata[1] = Iin * Current_coefficient;
    vofa_JustFloat.fdata[2] = VOUT * voltage_coefficient;
    vofa_JustFloat.fdata[3] = Iout * Io_Current_coefficient;
    vofa_JustFloat.fdata[4] = mppt.get_mppt_open_circuit_voltage_flag;
    vofa_JustFloat.fdata[5] = mppt.mppt_open_circuit_voltage * voltage_coefficient;
    vofa_JustFloat.fdata[6] = VIN;
    vofa_JustFloat.fdata[7] = protect_flag;
    vofa_JustFloat.fdata[8] = mppt.mppt_vin_target;
    vofa_JustFloat.fdata[9] = mppt.mppt_vin_error;
    vofa_JustFloat.fdata[10] = mppt.mppt_contral_output;
    vofa_JustFloat.fdata[11] = mppt.mppt_vin_error_integral;
    vofa_JustFloat.fdata[12] = 0;
    vofa_JustFloat.fdata[13] = 0;
    vofa_JustFloat.fdata[14] = 0;
    vofa_JustFloat.fdata[15] = 0;
    vofa_JustFloat.fdata[16] = 0;
    vofa_JustFloat.fdata[17] = 0;
    vofa_JustFloat.fdata[18] = 0;
    vofa_JustFloat.fdata[19] = 0;
    vofa_JustFloat.fdata[21] = 0;
    vofa_JustFloat.fdata[22] = 0;
    vofa_JustFloat.fdata[23] = 0;
    memcpy(vofa_data, vofa_JustFloat.fdata, buffer_size);
    HAL_UART_Transmit_DMA(&huart1, vofa_data, buffer_size);
}


#endif

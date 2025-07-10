#include <string.h>
#include "uart_to_printf.h"
#include "pid.h"

#ifdef __GNUC__
#define uart_to_printf int  __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif


uart_to_printf {
    HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, 0xFFFF);
    return ch;
}


///增加数据通道要记得初始化时加上0，避免数据帧尾错误
vofa_JustFloat_t vofa_JustFloat = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                   0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0x00, 0x00, 0x80, 0x7f};
unsigned char vofa_data[buffer_size];

uint32_t task_run = 0;



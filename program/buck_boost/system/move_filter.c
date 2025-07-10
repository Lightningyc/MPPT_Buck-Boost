//
// Created by 黄 on 2024/5/25.
//


#include "move_filter.h"
#include "main.h"
#include "pid.h"
//平滑均值滤波
#define data_length 12

/**
 * 移动平均滤波器函数
 * 
 * 该函数通过计算移动平均值来过滤输入数据，旨在平滑序列中的噪声。
 * 使用静态变量来保持滤波器的状态，确保在多次调用之间保持数据。
 * 
 * @param input_data 输入的待过滤数据，必须是无符号32位整数。
 * @return 返回经过移动平均滤波后的浮点数结果。
 */
float V_advalue_moveAverageFilter(uint32_t input_data) {
    // 初始化标志位，表示滤波器是否处于开始状态
    static uint8_t begin_flag = 0;
    // 用于累加输入数据的总和
    static uint32_t sum = 0;
    // 记录已经处理的数据数量
    static uint8_t count = 0;
    // 存储过滤后的值
    static float value_after_filter = 0;

    // 如果是滤波器的开始状态
    if (begin_flag == 0) {
        // 累加输入数据到总和中
        sum = sum + input_data;
        // 增加已处理数据的数量
        count++;
        // 当达到预设的数据长度时，切换到正常工作状态
        if (count == data_length) {
            // 设置标志位，表示滤波器开始正常工作
            begin_flag = 1;
            // 重置已处理数据的数量
            count = 0;
        }
    } else if (begin_flag == 1) {
        // 从总和中减去旧的平均值
        sum = sum - sum / data_length;
        // 加入新的输入数据
        sum = sum + input_data;
        // 计算新的平均值
        value_after_filter = (float) sum / data_length;
    }

    // 返回经过滤波后的值
    return value_after_filter;
}



float A_advalue_moveAverageFilter(uint32_t input_data) {
    static uint8_t begin_flag = 0;
    static uint32_t sum = 0;
    static uint8_t count = 0;
    static float value_after_filter = 0;
    if (begin_flag == 0) {
        sum = sum + input_data;
        count++;
        if (count == data_length) {
            begin_flag = 1;
            count = 0;
        }
    } else if (begin_flag == 1) {
        sum = sum - sum / data_length;
        sum = sum + input_data;
        value_after_filter = (float) sum / data_length;
    }
    return value_after_filter;

}

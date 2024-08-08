// encode GB2312
// my_config.h -- 自定义的配置文件（主要是工程相关的配置）
#ifndef __MY_CONFIG_H
#define __MY_CONFIG_H

#include "include.h" // 使用芯片官方提供的头文件

// 开发平台
// DEVELOPMENT_BOARD -- 开发板（DEMO板）
// CIRCUIT_BOARD -- 目标电路板
#define CIRCUIT_BOARD

// 是否打开调试
#define USE_MY_DEBUG            0

// 串口0波特率
#define UART0_BAUDRATE          115200

// 这款芯片（TX3260）不能使用UART1的DMA来接收不定长的数据
// 串口1波特率
#define UART1_BAUDRATE          115200

// 是否使用对应的片上外设/功能
#define USE_PRINTF_UART0        0   // printf()函数调用串口0的发送函数
#define USE_PRINTF_UART1        0   // printf()函数调用串口1的发送函数
#define USE_UART0               0
#define USE_UART1               1

// // 低功耗模式
// #define USE_IDLE_MODE           0   // 使用待机模式
// #define USE_STOP_MODE           0   // 使用停止模式

#include <stdio.h>   // printf()
#include <string.h>  // memset()
#include "my_gpio.h" // 自定义的、使用到的引脚
#include "uart0.h"
#include "uart1.h"
#include "tmr0.h" // 驱动黄灯的PWM
#include "tmr1.h" // 驱动白灯的PWM
#include "tmr2.h" // 每隔一定时间触发一次中断，用于调整灯光亮度

#include "light.h" // 电源指示灯的驱动
#include "send_data.h" // 单线发送数据的头文件
#include "my_sys.h" // 自定义的项目相关的文件（涉及更具体的项目的配置）
#include "adc.h" // adc，检测旋钮
#include "knob.h" // 旋钮相关的处理函数

// #include "idle_mode.h" // 待机模式
// #include "stop_mode.h" // 停止模式



#endif // end file

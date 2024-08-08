#ifndef __UART0_H
#define __UART0_H

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

#if USE_UART0

// 串口0波特率
#ifndef UART0_BAUDRATE
#define UART0_BAUDRATE 115200
#endif // UART0_BAUDRATE

// 串口0接收缓冲区的大小
// #ifndef UART0_RXBUF_LEN
// #define UART0_RXBUF_LEN 250
// #endif

// 用来存放接收的数据的缓冲区
// extern volatile u8 uart0_recv_buf[UART0_RXBUF_LEN];
// extern volatile u16 uart0_recv_cnt; // 接收缓冲区计数值
// extern volatile bit uart0_recv_flag; // 接收完成一帧的标志位

void uart0_config(void);
void uart0_sendbyte(u8 senddata);

#endif // USE_UART0

#endif

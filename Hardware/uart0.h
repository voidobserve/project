#ifndef __UART0_H
#define __UART0_H

#include "include.h"   // ʹ��оƬ�ٷ��ṩ��ͷ�ļ�
#include "my_config.h" // �����Զ����ͷ�ļ�

#if USE_UART0

// ����0������
#ifndef UART0_BAUDRATE
#define UART0_BAUDRATE 115200
#endif // UART0_BAUDRATE

// ����0���ջ������Ĵ�С
// #ifndef UART0_RXBUF_LEN
// #define UART0_RXBUF_LEN 250
// #endif

// ������Ž��յ����ݵĻ�����
// extern volatile u8 uart0_recv_buf[UART0_RXBUF_LEN];
// extern volatile u16 uart0_recv_cnt; // ���ջ���������ֵ
// extern volatile bit uart0_recv_flag; // �������һ֡�ı�־λ

void uart0_config(void);
void uart0_sendbyte(u8 senddata);

#endif // USE_UART0

#endif

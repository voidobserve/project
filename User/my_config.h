// encode GB2312
// my_config.h -- �Զ���������ļ�����Ҫ�ǹ�����ص����ã�
#ifndef __MY_CONFIG_H
#define __MY_CONFIG_H

#include "include.h" // ʹ��оƬ�ٷ��ṩ��ͷ�ļ�

// ����ƽ̨
// DEVELOPMENT_BOARD -- �����壨DEMO�壩
// CIRCUIT_BOARD -- Ŀ���·��
#define CIRCUIT_BOARD

// �Ƿ�򿪵���
#define USE_MY_DEBUG            0

// ����0������
#define UART0_BAUDRATE          115200

// ���оƬ��TX3260������ʹ��UART1��DMA�����ղ�����������
// ����1������
#define UART1_BAUDRATE          115200

// �Ƿ�ʹ�ö�Ӧ��Ƭ������/����
#define USE_PRINTF_UART0        0   // printf()�������ô���0�ķ��ͺ���
#define USE_PRINTF_UART1        0   // printf()�������ô���1�ķ��ͺ���
#define USE_UART0               0
#define USE_UART1               1

// // �͹���ģʽ
// #define USE_IDLE_MODE           0   // ʹ�ô���ģʽ
// #define USE_STOP_MODE           0   // ʹ��ֹͣģʽ

#include <stdio.h>   // printf()
#include <string.h>  // memset()
#include "my_gpio.h" // �Զ���ġ�ʹ�õ�������
#include "uart0.h"
#include "uart1.h"
#include "tmr0.h" // �����ƵƵ�PWM
#include "tmr1.h" // �����׵Ƶ�PWM
#include "tmr2.h" // ÿ��һ��ʱ�䴥��һ���жϣ����ڵ����ƹ�����

#include "light.h" // ��Դָʾ�Ƶ�����
#include "send_data.h" // ���߷������ݵ�ͷ�ļ�
#include "my_sys.h" // �Զ������Ŀ��ص��ļ����漰���������Ŀ�����ã�
#include "adc.h" // adc�������ť
#include "knob.h" // ��ť��صĴ�����

// #include "idle_mode.h" // ����ģʽ
// #include "stop_mode.h" // ֹͣģʽ



#endif // end file

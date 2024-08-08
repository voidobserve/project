#ifndef __ADC_H
#define __ADC_H

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

extern volatile u16 adc_val;       // adc值，0~4095

void adc_config(void);
void adc_getval(void);// 采集多次adc值，最终得到滤波后的adc值

#if USE_MY_DEBUG
void adc_showval(void);
#endif // USE_MY_DEBUG

#endif

#ifndef __TMR1_H
#define __TMR1_H

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件

void tmr1_pwm_config(void);  
void tmr1_enable(void);  // 开启定时器
void tmr1_disable(void); // 关闭定时器，清除硬件的计数值

void tmr1_set_pwm_duty(u16 duty); // 调节PWM占空比

#endif

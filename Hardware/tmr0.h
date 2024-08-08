#ifndef __TMR0_H
#define __TMR0_H

#include "include.h" // 使用芯片官方提供的头文件

#include "my_config.h" // 包含自定义的头文件

void tmr0_pwm_config(void);
void tmr0_enable(void);
void tmr0_disable(void);

void tmr0_set_pwm_duty(u16 duty);

#endif

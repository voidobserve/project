#ifndef __TMR0_H
#define __TMR0_H

#include "include.h" // ʹ��оƬ�ٷ��ṩ��ͷ�ļ�

#include "my_config.h" // �����Զ����ͷ�ļ�

void tmr0_pwm_config(void);
void tmr0_enable(void);
void tmr0_disable(void);

void tmr0_set_pwm_duty(u16 duty);

#endif

#ifndef __TMR1_H
#define __TMR1_H

#include "include.h"   // ʹ��оƬ�ٷ��ṩ��ͷ�ļ�
#include "my_config.h" // �����Զ����ͷ�ļ�

void tmr1_pwm_config(void);  
void tmr1_enable(void);  // ������ʱ��
void tmr1_disable(void); // �رն�ʱ�������Ӳ���ļ���ֵ

void tmr1_set_pwm_duty(u16 duty); // ����PWMռ�ձ�

#endif

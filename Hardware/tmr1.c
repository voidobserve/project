#include "tmr1.h"

#define TMR1_CNT_TIME (SYSCLK / 8 / 32000 - 1) // �������� 32KHz��xxms����һ���ж�

void tmr1_pwm_config(void)
{
    TMR_ALLCON = TMR1_CNT_CLR(0x1);                           // �������ֵ
    TMR1_PRH = TMR_PERIOD_VAL_H((TMR1_CNT_TIME >> 8) & 0xFF); // ����ֵ
    TMR1_PRL = TMR_PERIOD_VAL_L((TMR1_CNT_TIME >> 0) & 0xFF);
    TMR1_PWMH = TMR_PWM_VAL_H(((TMR1_CNT_TIME / 2) >> 8) & 0xFF); // ռ�ձ�����ֵ��TMR1_CNT_TIME / 2������50%ռ�ձȣ�
    TMR1_PWML = TMR_PWM_VAL_L(((TMR1_CNT_TIME / 2) >> 0) & 0xFF);
    TMR1_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                               // ʹ�ܼ����ж�
    TMR1_CONL = /*TMR_SOURCE_SEL(0x7) |*/ TMR_PRESCALE_SEL(0x03) | TMR_MODE_SEL(0x2); // ѡ��ϵͳʱ�ӣ�16��Ƶ��PWMģʽ
    TMR1_CONL |= TMR_SOURCE_SEL(0x05);                                                // ���ö�ʱ����ʱ��Դ�������κ�ʱ��
}

/**
 * @brief ������ʱ��TMR1����ʼ��ʱ
 */
void tmr1_enable(void)
{
    // ���¸�TMR1����ʱ��
    TMR1_CONL &= ~(TMR_SOURCE_SEL(0x07)); // �����ʱ����ʱ��Դ���üĴ���
    TMR1_CONL |= TMR_SOURCE_SEL(0x06);    // ���ö�ʱ����ʱ��Դ��ʹ��ϵͳʱ��
}

/**
 * @brief �رն�ʱ������ռ���ֵ
 */
void tmr1_disable(void)
{
    // ������ʱ���ṩʱ�ӣ�����ֹͣ����
    TMR1_CONL &= ~(TMR_SOURCE_SEL(0x07)); // �����ʱ����ʱ��Դ���üĴ���
    TMR1_CONL |= TMR_SOURCE_SEL(0x05);    // ���ö�ʱ����ʱ��Դ�������κ�ʱ��

    // �����ʱ���ļ���ֵ
    TMR1_CNTL = 0;
    TMR1_CNTH = 0;
}

// ������duty--ռ�ձȣ�����ʱ����С�������λ����������duty=234��ռ�ձ�Ϊ23.4%
void tmr1_set_pwm_duty(u16 duty)
{
    u16 tmp = TMR1_CNT_TIME * duty / 1000;
    TMR1_PWMH = TMR_PWM_VAL_H(((tmp) >> 8) & 0xFF); // ռ�ձ�����ֵ��TMR1_CNT_TIME / tmp������duty%ռ�ձȣ�
    TMR1_PWML = TMR_PWM_VAL_L(((tmp) >> 0) & 0xFF);
}

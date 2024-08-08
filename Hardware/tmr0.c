#include "tmr0.h"

#define TMR0_CNT_TIME (SYSCLK / 8 / 32000 - 1) // �������� 32KHz��xxms����һ���ж�

void tmr0_pwm_config(void)
{
    TMR_ALLCON = TMR0_CNT_CLR(0x1);                           // �������ֵ
    TMR0_PRH = TMR_PERIOD_VAL_H((TMR0_CNT_TIME >> 8) & 0xFF); // ����ֵ
    TMR0_PRL = TMR_PERIOD_VAL_L((TMR0_CNT_TIME >> 0) & 0xFF);
    TMR0_PWMH = TMR_PWM_VAL_H(((TMR0_CNT_TIME / 2) >> 8) & 0xFF); // ռ�ձ�����ֵ��TMR0_CNT_TIME / 2������50%ռ�ձȣ�
    TMR0_PWML = TMR_PWM_VAL_L(((TMR0_CNT_TIME / 2) >> 0) & 0xFF);
    TMR0_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                               // ʹ�ܼ����ж�
    TMR0_CONL = /*TMR_SOURCE_SEL(0x7) |*/ TMR_PRESCALE_SEL(0x03) | TMR_MODE_SEL(0x2); // ѡ��ϵͳʱ�ӣ�16��Ƶ��PWMģʽ
    TMR0_CONL |= TMR_SOURCE_SEL(0x05);                                                // ���ö�ʱ����ʱ��Դ�������κ�ʱ��
}

/**
 * @brief ������ʱ��TMR0����ʼ��ʱ
 */
void tmr0_enable(void)
{
    // ���¸�TMR0����ʱ��
    TMR0_CONL &= ~(TMR_SOURCE_SEL(0x07)); // �����ʱ����ʱ��Դ���üĴ���
    TMR0_CONL |= TMR_SOURCE_SEL(0x06);    // ���ö�ʱ����ʱ��Դ��ʹ��ϵͳʱ��
}

/**
 * @brief �رն�ʱ��0����ռ���ֵ
 */
void tmr0_disable(void)
{
    // ������ʱ���ṩʱ�ӣ�����ֹͣ����
    TMR0_CONL &= ~(TMR_SOURCE_SEL(0x07)); // �����ʱ����ʱ��Դ���üĴ���
    TMR0_CONL |= TMR_SOURCE_SEL(0x05);    // ���ö�ʱ����ʱ��Դ�������κ�ʱ��
    // �����ʱ���ļ���ֵ
    TMR0_CNTL = 0;
    TMR0_CNTH = 0;
}

// ������duty--ռ�ձȣ�����ʱ����С�������λ����������duty=234��ռ�ձ�Ϊ23.4%
void tmr0_set_pwm_duty(u16 duty)
{
    u16 tmp = TMR0_CNT_TIME * duty / 1000;
    TMR0_PWMH = TMR_PWM_VAL_H(((tmp) >> 8) & 0xFF); // ռ�ձ�����ֵ��TMR0_CNT_TIME / tmp������duty%ռ�ձȣ�
    TMR0_PWML = TMR_PWM_VAL_L(((tmp) >> 0) & 0xFF);
}

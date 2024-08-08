#include "tmr1.h"

#define TMR1_CNT_TIME (SYSCLK / 8 / 32000 - 1) // 计数周期 32KHz，xxms产生一次中断

void tmr1_pwm_config(void)
{
    TMR_ALLCON = TMR1_CNT_CLR(0x1);                           // 清除计数值
    TMR1_PRH = TMR_PERIOD_VAL_H((TMR1_CNT_TIME >> 8) & 0xFF); // 周期值
    TMR1_PRL = TMR_PERIOD_VAL_L((TMR1_CNT_TIME >> 0) & 0xFF);
    TMR1_PWMH = TMR_PWM_VAL_H(((TMR1_CNT_TIME / 2) >> 8) & 0xFF); // 占空比设置值（TMR1_CNT_TIME / 2，等于50%占空比）
    TMR1_PWML = TMR_PWM_VAL_L(((TMR1_CNT_TIME / 2) >> 0) & 0xFF);
    TMR1_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                               // 使能计数中断
    TMR1_CONL = /*TMR_SOURCE_SEL(0x7) |*/ TMR_PRESCALE_SEL(0x03) | TMR_MODE_SEL(0x2); // 选择系统时钟，16分频，PWM模式
    TMR1_CONL |= TMR_SOURCE_SEL(0x05);                                                // 配置定时器的时钟源，不用任何时钟
}

/**
 * @brief 开启定时器TMR1，开始计时
 */
void tmr1_enable(void)
{
    // 重新给TMR1配置时钟
    TMR1_CONL &= ~(TMR_SOURCE_SEL(0x07)); // 清除定时器的时钟源配置寄存器
    TMR1_CONL |= TMR_SOURCE_SEL(0x06);    // 配置定时器的时钟源，使用系统时钟
}

/**
 * @brief 关闭定时器，清空计数值
 */
void tmr1_disable(void)
{
    // 不给定时器提供时钟，让它停止计数
    TMR1_CONL &= ~(TMR_SOURCE_SEL(0x07)); // 清除定时器的时钟源配置寄存器
    TMR1_CONL |= TMR_SOURCE_SEL(0x05);    // 配置定时器的时钟源，不用任何时钟

    // 清除定时器的计数值
    TMR1_CNTL = 0;
    TMR1_CNTH = 0;
}

// 参数：duty--占空比，传参时包括小数点后两位的数，例如duty=234，占空比为23.4%
void tmr1_set_pwm_duty(u16 duty)
{
    u16 tmp = TMR1_CNT_TIME * duty / 1000;
    TMR1_PWMH = TMR_PWM_VAL_H(((tmp) >> 8) & 0xFF); // 占空比设置值（TMR1_CNT_TIME / tmp，等于duty%占空比）
    TMR1_PWML = TMR_PWM_VAL_L(((tmp) >> 0) & 0xFF);
}

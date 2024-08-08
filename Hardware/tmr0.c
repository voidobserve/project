#include "tmr0.h"

#define TMR0_CNT_TIME (SYSCLK / 8 / 32000 - 1) // 计数周期 32KHz，xxms产生一次中断

void tmr0_pwm_config(void)
{
    TMR_ALLCON = TMR0_CNT_CLR(0x1);                           // 清除计数值
    TMR0_PRH = TMR_PERIOD_VAL_H((TMR0_CNT_TIME >> 8) & 0xFF); // 周期值
    TMR0_PRL = TMR_PERIOD_VAL_L((TMR0_CNT_TIME >> 0) & 0xFF);
    TMR0_PWMH = TMR_PWM_VAL_H(((TMR0_CNT_TIME / 2) >> 8) & 0xFF); // 占空比设置值（TMR0_CNT_TIME / 2，等于50%占空比）
    TMR0_PWML = TMR_PWM_VAL_L(((TMR0_CNT_TIME / 2) >> 0) & 0xFF);
    TMR0_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                               // 使能计数中断
    TMR0_CONL = /*TMR_SOURCE_SEL(0x7) |*/ TMR_PRESCALE_SEL(0x03) | TMR_MODE_SEL(0x2); // 选择系统时钟，16分频，PWM模式
    TMR0_CONL |= TMR_SOURCE_SEL(0x05);                                                // 配置定时器的时钟源，不用任何时钟
}

/**
 * @brief 开启定时器TMR0，开始计时
 */
void tmr0_enable(void)
{
    // 重新给TMR0配置时钟
    TMR0_CONL &= ~(TMR_SOURCE_SEL(0x07)); // 清除定时器的时钟源配置寄存器
    TMR0_CONL |= TMR_SOURCE_SEL(0x06);    // 配置定时器的时钟源，使用系统时钟
}

/**
 * @brief 关闭定时器0，清空计数值
 */
void tmr0_disable(void)
{
    // 不给定时器提供时钟，让它停止计数
    TMR0_CONL &= ~(TMR_SOURCE_SEL(0x07)); // 清除定时器的时钟源配置寄存器
    TMR0_CONL |= TMR_SOURCE_SEL(0x05);    // 配置定时器的时钟源，不用任何时钟
    // 清除定时器的计数值
    TMR0_CNTL = 0;
    TMR0_CNTH = 0;
}

// 参数：duty--占空比，传参时包括小数点后两位的数，例如duty=234，占空比为23.4%
void tmr0_set_pwm_duty(u16 duty)
{
    u16 tmp = TMR0_CNT_TIME * duty / 1000;
    TMR0_PWMH = TMR_PWM_VAL_H(((tmp) >> 8) & 0xFF); // 占空比设置值（TMR0_CNT_TIME / tmp，等于duty%占空比）
    TMR0_PWML = TMR_PWM_VAL_L(((tmp) >> 0) & 0xFF);
}

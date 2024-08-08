// encode UTF-8
// 指示灯的驱动源程序
#include "light.h"

// 检测灯光控制的模式
void check_light_ctl_mode(void)
{
#ifdef CIRCUIT_BOARD
    p27_input_config(); // P27，与黄灯相连的引脚配置成输入上拉
    if (P27)
    {
        light_ctl_mode = LIGHT_CTL_MODE_SINGLE; // 接下来触摸按键可以控制开关灯（light_ctl_mode = 1）
    }
    else
    {
        light_ctl_mode = LIGHT_CTL_MODE_MUILTY; // 接下来触摸按键不仅可以控制开关灯，还可以控制三种颜色灯光的切换（light_ctl_mode = 0）
    }
#endif // #ifdef CIRCUIT_BOARD
#ifdef DEVELOPMENT_BOARD
    p22_input_config(); // P22输入上拉
    if (P22)
    {
        light_ctl_mode = LIGHT_CTL_MODE_SINGLE; // 接下来触摸按键可以控制开关灯（light_ctl_mode = 1）
    }
    else
    {
        light_ctl_mode = LIGHT_CTL_MODE_MUILTY; // 接下来触摸按键不仅可以控制开关灯，还可以控制三种颜色灯光的切换（light_ctl_mode = 0）
        // light_muilty_cur_status = LIGHT_MUILTY_CUR_OFF; // 灯的状态为关闭（在这里给变量赋初值）
    }
#endif // #ifdef DEVELOPMENT_BOARD

    // 检测完成后，将IO配置回PWM输出

#ifdef CIRCUIT_BOARD
    // 配置P27为timer0的PWM输出端口
    P2_MD1 &= ~GPIO_P27_MODE_SEL(0x3);
    P2_MD1 |= GPIO_P27_MODE_SEL(0x1);
    FOUT_S27 = GPIO_FOUT_TMR0_PWMOUT;
#endif // #ifdef CIRCUIT_BOARD
#ifdef DEVELOPMENT_BOARD
    // 使用demo板上的引脚进行测试
    P2_MD0 &= ~GPIO_P22_MODE_SEL(0x3);
    P2_MD0 |= GPIO_P22_MODE_SEL(0x1); // 输出模式
    FOUT_S22 = GPIO_FOUT_TMR0_PWMOUT;
#endif // #ifdef DEVELOPMENT_BOARD

    tmr0_pwm_config();
}

// 驱动白灯的PWM配置
void white_light_pwm_config(void)
{
#ifdef CIRCUIT_BOARD
    P2_MD0 &= ~GPIO_P21_MODE_SEL(0x3);
    P2_MD0 |= GPIO_P21_MODE_SEL(0x1); // 输出模式
    FOUT_S21 = GPIO_FOUT_TMR1_PWMOUT;
#endif // #ifdef CIRCUIT_BOARD
#ifdef DEVELOPMENT_BOARD
    // 使用demo板上的P23引脚进行测试
    P2_MD0 &= ~GPIO_P23_MODE_SEL(0x3);
    P2_MD0 |= GPIO_P23_MODE_SEL(0x1); // 输出模式
    FOUT_S23 = GPIO_FOUT_TMR1_PWMOUT;
#endif // #ifdef DEVELOPMENT_BOARD

    tmr1_pwm_config();
}

// 关闭黄灯（不能通过直接设置占空比为0来关灯，实际的波形还是会有百分之零点几的分量）
void yellow_light_close(void)
{
#if 0  // 通过延时的方式来调节灯光亮度
    cur_pwm_duty = 700; // 单色模式下，灯光的最大亮度为70%占空比对应的亮度
    for (; cur_pwm_duty > 0; cur_pwm_duty--)
    {
        tmr0_set_pwm_duty(cur_pwm_duty);
        delay_ms(1);
    }
    // 最后，将PWM设置为0
    // cur_pwm_duty = 0;
    tmr0_set_pwm_duty(cur_pwm_duty);
    tmr0_disable();
#endif // 通过延时的方式来调节灯光亮度

    tmr0_disable();

#ifdef CIRCUIT_BOARD
    FOUT_S27 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出
    P27 = 0;
#endif // #ifdef CIRCUIT_BOARD
#ifdef DEVELOPMENT_BOARD
    FOUT_S22 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出
    P22 = 0;
#endif // #ifdef DEVELOPMENT_BOARD
}

// 关闭白灯（不能通过直接设置占空比为0来关灯，实际的波形还是会有百分之零点几的分量）
void white_light_close(void)
{
    tmr1_disable();

#ifdef CIRCUIT_BOARD
    FOUT_S21 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出
    P21 = 0;
#endif // #ifdef  CIRCUIT_BOARD
#ifdef DEVELOPMENT_BOARD
    FOUT_S23 = GPIO_FOUT_AF_FUNC; // 选择AF功能输出
    P23 = 0;
#endif //     #ifdef DEVELOPMENT_BOARD
}

// 开启黄灯（开启前需要注意先设置好PWM的占空比）
void yellow_light_open(void)
{
#ifdef DEVELOPMENT_BOARD
    FOUT_S22 = GPIO_FOUT_TMR0_PWMOUT; // 将IO复用为定时器0的PWM输出
#endif                                // #ifdef DEVELOPMENT_BOARD
#ifdef CIRCUIT_BOARD
    FOUT_S27 = GPIO_FOUT_TMR0_PWMOUT; // 将IO复用为定时器0的PWM输出
#endif                                // #ifdef CIRCUIT_BOARD
    tmr0_enable();

#if 0  // 通过延时的方式来调节灯光亮度
    cur_pwm_duty = 0;
    tmr0_set_pwm_duty(cur_pwm_duty); // 先将占空比设置为0，再开启定时器
    tmr0_enable();
    // pwm从0.0%变化到70.0%
    for (cur_pwm_duty = 0; cur_pwm_duty <= LIGHT_OPEN_CLOSE_TIME; cur_pwm_duty++)
    {
        tmr0_set_pwm_duty(cur_pwm_duty);
        delay_ms(1);
    }
#endif // 通过延时的方式来调节灯光亮度
}

// 开启白灯（开启前需要注意先设置好PWM的占空比）
void white_light_open(void)
{
#ifdef DEVELOPMENT_BOARD
    FOUT_S23 = GPIO_FOUT_TMR1_PWMOUT; // 将IO复用为定时器1的PWM输出
#endif                                // #ifdef DEVELOPMENT_BOARD
#ifdef CIRCUIT_BOARD
    FOUT_S21 = GPIO_FOUT_TMR1_PWMOUT; // 将IO复用为定时器1的PWM输出
#endif                                // #ifdef  CIRCUIT_BOARD

    tmr1_enable();
}


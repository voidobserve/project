// 定时器TMR2的驱动源文件
#include "tmr2.h"

#define TMR2_CNT_TIME (SYSCLK / 32 / 10000 - 1) // 计数周期 10KHz，100us产生一次中断

// volatile bit tmr2_flag = 0; // tmr2中断服务函数中会置位的标志位
volatile u32 tmr2_cnt = 0; // 定时器TMR2的计数值（每次在中断服务函数中会加一）

/**
 * @brief 配置定时器TMR2
 */
void tmr2_config(void)
{
    TMR_ALLCON = TMR2_CNT_CLR(0x1);                           // 清除计数值
    TMR2_PRH = TMR_PERIOD_VAL_H((TMR2_CNT_TIME >> 8) & 0xFF); // 周期值
    TMR2_PRL = TMR_PERIOD_VAL_L((TMR2_CNT_TIME >> 0) & 0xFF);
    TMR2_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                               // 清除TMR2的计数标志位，表示未完成计数. 使能TMR2的计数中断
    TMR2_CONL = /* TMR_SOURCE_SEL(0x7) |*/ TMR_PRESCALE_SEL(0x5) | TMR_MODE_SEL(0x1); // 选择系统时钟，32分频，计数模式
    TMR2_CONL |= TMR_SOURCE_SEL(0x05);                                                // 配置TMR2的时钟源，不用任何时钟
    IE_EA = 1;                                                                        // 打开总中断
}

/**
 * @brief 开启定时器TMR2，开始计时
 */
void tmr2_enable(void)
{
    // 重新给TMR2配置时钟
    TMR2_CONL &= ~(TMR_SOURCE_SEL(0x07)); // 清除定时器的时钟源配置寄存器
    TMR2_CONL |= TMR_SOURCE_SEL(0x06);    // 配置定时器的时钟源，使用系统时钟（约21MHz）

    __EnableIRQ(TMR2_IRQn); // 使能中断
    IE_EA = 1;              // 打开总中断
}

/**
 * @brief 关闭定时器2，清空计数值
 */
void tmr2_disable(void)
{
    // 不给定时器提供时钟，让它停止计数
    TMR2_CONL &= ~(TMR_SOURCE_SEL(0x07)); // 清除定时器的时钟源配置寄存器
    TMR2_CONL |= TMR_SOURCE_SEL(0x05);    // 配置定时器的时钟源，不用任何时钟

    // 清除定时器的计数值
    TMR2_CNTL = 0;
    TMR2_CNTH = 0;

    __DisableIRQ(TMR2_IRQn); // 关闭中断（不使能中断）

    tmr2_cnt = 0; // 清除变量的计数值
}

// TMR2中断服务函数
void TIMR2_IRQHandler(void) interrupt TMR2_IRQn
{
    static bit is_white_light_open = 0; // 标志位，用于控制白灯的开启，防止多次调用开启函数，造成闪烁

    // 进入中断设置IP，不可删除
    __IRQnIPnPush(TMR2_IRQn);
    // ---------------- 用户函数处理 -------------------
    if (TMR2_CONH & TMR_PRD_PND(0x1))
    {
        TMR2_CONH |= TMR_PRD_PND(0x1); // 清除pending
        // P24 = ~P24; // 测试定时器触发中断的周期
        tmr2_cnt++; //

        // 根据控制灯光的标志位来进行相应的操作
        switch (light_control_flag)
        {
#if 0                          // 开启/关闭黄灯
            // case LIGHT_YELLOW_OPEN: // 开启黄灯
            //     yellow_light_open();
            //     if (cur_pwm_duty < 700)
            //     {
            //         if (tmr2_cnt >= 10) // 每过1ms，给PWM占空比增加0.1%
            //         {
            //             tmr2_cnt = 0;
            //             tmr0_set_pwm_duty(++cur_pwm_duty);
            //         }
            //     }
            //     else
            //     {
            //         // 调整完成后，清除标志位
            //         light_control_flag = LIGHT_CONTROL_NONE;
            //     }
            //     break;
            // case LIGHT_YELLOW_CLOSE: // 关闭黄灯
            //     if (cur_pwm_duty > 0)
            //     {
            //         if (tmr2_cnt >= 10) // 每过1ms，给PWM占空比减少0.1%
            //         {
            //             tmr2_cnt = 0;
            //             tmr0_set_pwm_duty(--cur_pwm_duty);
            //         }
            //     }
            //     else
            //     {
            //         yellow_light_close(); //
            //         // 调整完成后，清除标志位
            //         light_control_flag = LIGHT_CONTROL_NONE;
            //     }
            //     break;
#endif                         // 开启/关闭黄灯
        case LIGHT_WHITE_OPEN: // 打开白灯
            
            if (0 == is_white_light_open)
            {
                white_light_open();
                is_white_light_open = 1;
            }
                
            if (cur_pwm_duty < cur_white_duty)
            {
                // if (tmr2_cnt >= 10) // 每过1ms，给PWM占空比增加0.1%
                if (tmr2_cnt >= PWM_ADJUST_DUTY_TIME_STEP) 
                {
                    tmr2_cnt = 0;
                    tmr1_set_pwm_duty(++cur_pwm_duty);
                }
            }
            else
            {
                // 调整完成后，清除标志位
                light_control_flag = LIGHT_CONTROL_NONE;
            }
            break;
        case LIGHT_WHITE_CLOSE: // 关闭白灯
            if (cur_pwm_duty > 0)
            {
                // if (tmr2_cnt >= 10) // 每过1ms，给PWM占空比减少0.1%
                if (tmr2_cnt >= PWM_ADJUST_DUTY_TIME_STEP) 
                {
                    tmr2_cnt = 0;
                    tmr1_set_pwm_duty(--cur_pwm_duty);
                }
            }
            else
            {
                white_light_close(); //
                
                // 调整完成后，清除标志位
                light_control_flag = LIGHT_CONTROL_NONE;
                is_white_light_open = 0;
            }
            break;

        default:          // 默认什么也不做
            tmr2_cnt = 0; // 清空存放定时器计数值的变量
            break;
        }

#if 0  // [不能实现功能的代码]把所有短按、长按的灯光调节都放在定时器，代码会特别长，而且不好处理
        if (is_touch_key_light_hold && tmr2_cnt >= 15)
        {
#if 1  // 进行长按的灯光调节（长按时，每1.5ms调整一次）
            if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode)
            {
                // 如果当前是单色模式（只控制白灯）
                if (light_adjust_add_sub_flag)
                {
                    // 如果当前是在增大亮度
                    if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                    {
                        tmr1_set_pwm_duty(++cur_pwm_duty);
                        white_light_open();
                    }
                    else
                    {
                        // 如果已经到了最大亮度
                        // cur_pwm_duty = 700;
                        tmr1_set_pwm_duty(cur_pwm_duty);
                        white_light_open();
                    }

                    light_on_off_status = 1; // 更新灯光状态
                }
                else
                {
                    // 如果当前是在减小亮度
                    if (cur_pwm_duty > 0)
                    {
                        tmr1_set_pwm_duty(--cur_pwm_duty);
                        white_light_open();

                        light_on_off_status = 1; // 更新灯光状态
                    }
                    else
                    {
                        // 如果已经到了最小亮度
                        tmr1_set_pwm_duty(cur_pwm_duty);
                        white_light_close();

                        light_on_off_status = 0; // 更新灯光状态
                    }
                }
            }
            else if (LIGHT_CTL_MODE_MUILTY == light_ctl_mode)
            {
                // 如果当前是三色切换模式
                // 判断当前是不是黄白色，如果是，需要注意最大的占空比为60%，如果不是，最大的占空比为70%

                if (LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status)
                {
                    // 如果当前是白灯
                    if (light_adjust_add_sub_flag)
                    {
                        // 如果当前是在增大亮度
                        if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                        {
                            tmr1_set_pwm_duty(++cur_pwm_duty);
                            white_light_open();
                        }
                        else
                        {
                            // 如果已经到了最大亮度
                            tmr1_set_pwm_duty(cur_pwm_duty);
                            white_light_open();
                        }

                        light_on_off_status = 1; // 更新灯光状态
                    }
                    else
                    {
                        // 如果当前是在减小亮度
                        if (cur_pwm_duty > 0)
                        {
                            tmr1_set_pwm_duty(--cur_pwm_duty);
                            white_light_open();
                            light_on_off_status = 1; // 更新灯光状态
                        }
                        else
                        {
                            // 如果已经到了最小亮度
                            tmr1_set_pwm_duty(cur_pwm_duty);
                            white_light_close();
                            light_on_off_status = 0; // 更新灯光状态
                        }
                    }
                }
                else if (LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status)
                {
                    // 如果当前是黄灯
                    if (light_adjust_add_sub_flag)
                    {
                        // 如果当前是在增大亮度
                        if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                        {
                            tmr0_set_pwm_duty(++cur_pwm_duty);
                            yellow_light_open();
                        }
                        else
                        {
                            // 如果已经到了最大亮度
                            tmr0_set_pwm_duty(cur_pwm_duty);
                            yellow_light_open();
                        }
                        light_on_off_status = 1; // 更新灯光状态
                    }
                    else
                    {
                        // 如果当前是在减小亮度
                        if (cur_pwm_duty > 0)
                        {
                            tmr0_set_pwm_duty(--cur_pwm_duty);
                            yellow_light_open();
                            light_on_off_status = 1; // 更新灯光状态
                        }
                        else
                        {
                            // 如果已经到了最小亮度
                            // cur_pwm_duty = 0;
                            tmr0_set_pwm_duty(cur_pwm_duty);
                            yellow_light_close();
                            light_on_off_status = 0; // 更新灯光状态
                        }
                    }
                }
                else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
                {
                    // 如果当前是黄白色
                    if (light_adjust_add_sub_flag)
                    {
                        // 如果当前是在增大亮度
                        if (cur_pwm_duty < PWM_MAX_DUTY_MUILTY_LIGHT)
                        {
                            tmr0_set_pwm_duty(++cur_pwm_duty);
                            tmr1_set_pwm_duty(cur_pwm_duty);
                            yellow_light_open();
                            white_light_open();
                        }
                        else
                        {
                            // 如果已经到了最大亮度
                            tmr0_set_pwm_duty(cur_pwm_duty);
                            tmr1_set_pwm_duty(cur_pwm_duty);
                            yellow_light_open();
                            white_light_open();
                        }
                        light_on_off_status = 1; // 更新灯光状态
                    }
                    else
                    {
                        // 如果当前是在减小亮度
                        if (cur_pwm_duty > 0)
                        {
                            tmr0_set_pwm_duty(--cur_pwm_duty);
                            tmr1_set_pwm_duty(cur_pwm_duty);
                            yellow_light_open();
                            white_light_open();
                            light_on_off_status = 1; // 更新灯光状态
                        }
                        else
                        {
                            // 如果已经到了最小亮度
                            tmr0_set_pwm_duty(cur_pwm_duty);
                            tmr1_set_pwm_duty(cur_pwm_duty);
                            yellow_light_close();
                            white_light_close();
                            light_on_off_status = 0; // 更新灯光状态
                        }
                    }
                }
                else if (LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status)
                {
                    // 如果当前灯是关闭的，长按后松开手时，把灯打开
                    is_touch_key_light_press = 1;
                    light_adjust_add_sub_flag = 1;
                }
            }
#endif // 进行长按的灯光调节
        }
        else if (0 == is_touch_key_light_hold) // 如果不是长按
        {
            // 根据控制灯光的标志位来进行相应的操作
            switch (light_control_flag)
            {
#if 0  // 开启/关闭黄灯
       // case LIGHT_YELLOW_OPEN: // 开启黄灯
       //     yellow_light_open();
       //     if (cur_pwm_duty < 700)
       //     {
       //         if (tmr2_cnt >= 10) // 每过1ms，给PWM占空比增加0.1%
       //         {
       //             tmr2_cnt = 0;
       //             tmr0_set_pwm_duty(++cur_pwm_duty);
       //         }
       //     }
       //     else
       //     {
       //         // 调整完成后，清除标志位
       //         light_control_flag = LIGHT_CONTROL_NONE;
       //     }
       //     break;
       // case LIGHT_YELLOW_CLOSE: // 关闭黄灯
       //     if (cur_pwm_duty > 0)
       //     {
       //         if (tmr2_cnt >= 10) // 每过1ms，给PWM占空比减少0.1%
       //         {
       //             tmr2_cnt = 0;
       //             tmr0_set_pwm_duty(--cur_pwm_duty);
       //         }
       //     }
       //     else
       //     {
       //         yellow_light_close(); //
       //         // 调整完成后，清除标志位
       //         light_control_flag = LIGHT_CONTROL_NONE;
       //     }
       //     break;
#endif // 开启/关闭黄灯
            case LIGHT_WHITE_OPEN: // 打开白灯
                white_light_open();
                if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                {
                    if (tmr2_cnt >= 10) // 每过1ms，给PWM占空比增加0.1%
                    {
                        tmr2_cnt = 0;
                        tmr1_set_pwm_duty(++cur_pwm_duty);
                    }
                }
                else
                {
                    // 调整完成后，清除标志位
                    light_control_flag = LIGHT_CONTROL_NONE;
                }
                break;
            case LIGHT_WHITE_CLOSE: // 关闭白灯
                if (cur_pwm_duty > 0)
                {
                    if (tmr2_cnt >= 10) // 每过1ms，给PWM占空比减少0.1%
                    {
                        tmr2_cnt = 0;
                        tmr1_set_pwm_duty(--cur_pwm_duty);
                    }
                }
                else
                {
                    white_light_close(); //
                    // 调整完成后，清除标志位
                    light_control_flag = LIGHT_CONTROL_NONE;
                }
                break;

            default:          // 默认什么也不做
                tmr2_cnt = 0; // 清空存放定时器计数值的变量
                break;
            }
        }
        // if (tmr2_cnt >= 100)
        // {
        //     tmr2_cnt = 0;

        //     if (light_on_off_status)
        //     {
        //         // 如果灯光已经开启
        //         if (cur_pwm_duty > 0)
        //         {
        //             tmr0_set_pwm_duty(cur_pwm_duty--);
        //         }
        //         else
        //         {
        //             tmr2_flag = 1; // 表示已经调整完成
        //         }
        //     }
        //     else
        //     {
        //         // 如果灯光已经关闭
        //         if (cur_pwm_duty < 700)
        //         {
        //             tmr0_set_pwm_duty(cur_pwm_duty++);
        //         }
        //         else
        //         {
        //             tmr2_flag = 1; // 表示已经调整完成
        //         }
        //     }
        // }
#endif
    }

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(TMR2_IRQn);
}

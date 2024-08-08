// encoding GB2312
#include "knob.h"

static volatile u16 last_adc_val = 0;         // 记录之前检测的adc值，如果有变化，则要立即调整灯光亮度
static volatile bit last_knob_on_off_pin = 0; // 记录旋钮开/关引脚的电平状态，如果有变化，说明要执行开灯/关灯操作
// 当前灯光是否为最暗的标志位，用于防止旋钮处于最小范围却未关闭时，
// 重复进入功能处理函数，导致灯光闪烁
// 0--否，1--是
static volatile bit is_light_lowest_flag = 0;

void knob_config(void)
{
    // 旋钮开/关引脚配置为上拉输入
    p15_input_config();
    // 用于检测旋钮一侧电压的adc的配置
    adc_config();

    adc_getval();
    last_adc_val = adc_val;                 //  第一次上电时，检测adc值（防止断电时旋钮没有关闭，结果一上电就点亮灯）
    last_knob_on_off_pin = KNOB_ON_OFF_PIN; // 第一次上电时，记录旋钮开/关引脚的电平状态
}

u16 __target_pwm_duty_update(u16 __target_pwm_duty)
{
    // 存放最终要调节到的pwm占空比，精确到小数点后1位
    // 例如 target_pwm_duty = 254，则占空比为25.4%
    u16 target_pwm_duty = 0;

    adc_getval(); // 得到一次滤波后的adc值

    if ((1 == is_light_lowest_flag && adc_val <= (ADC_MIN_VAL + ADC_STEP_VAL)) ||
        (last_adc_val <= (adc_val + ADC_STEP_VAL) && (last_adc_val >= (adc_val - ADC_STEP_VAL))))
    {
        // 如果测得的数值对应旋钮的最小位置附近，同样退出函数
        // 如果数值在 Δ 以内，则认为没有变化，不进行相应的处理操作
        return __target_pwm_duty; // 返回原来的数值
    }

    last_adc_val = adc_val; //  记录之前检测的adc值

    // 得到当前adc值占总电压值的百分比，精确到小数点后1位
    // （当前检测到的电压值占总电压值的百分比 刚好对应 亮度的百分比）
    target_pwm_duty = (u32)adc_val * 700 / ADC_MAX_VAL;

    // 如果计算的目标占空比超出了当前支持的最大占空比，按最大的占空比处理
    if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode ||
        LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status ||
        LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status)
    {
        if (target_pwm_duty > PWM_MAX_DUTY_SINGLE_LIGHT)
        {
            target_pwm_duty = PWM_MAX_DUTY_SINGLE_LIGHT;
        }
    }
    else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
    {
        if (target_pwm_duty > PWM_MAX_DUTY_MUILTY_LIGHT)
        {
            target_pwm_duty = PWM_MAX_DUTY_MUILTY_LIGHT;
        }
    }

    if (adc_val <= ADC_MIN_VAL) // adc值小于设定的最小值区间，一律按照1.0%的占空比来设置
    {
        u8 i = 0;
        for (i = 0; i < 3; i++)
        {
            adc_getval();
            if (!(adc_val <= ADC_MIN_VAL))
            {
                is_light_lowest_flag = 0;
                return __target_pwm_duty;
            }
        }

        if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
        {
            target_pwm_duty = PWM_MIN_DUTY_YELLOW_WHITE_LIGHT;
        }
        else
        {
            target_pwm_duty = PWM_MIN_DUTY_LIGHT;
        }

        is_light_lowest_flag = 1;
        last_adc_val = target_pwm_duty * ADC_MAX_VAL / 700 / adc_val;
    }
    else
    {
        is_light_lowest_flag = 0;
    }

    return target_pwm_duty;
}

// 旋钮相关的处理函数
void knob_handle(void)
{
    u8 i = 0;

    // 存放最终要调节到的pwm占空比，精确到小数点后1位
    // 例如 target_pwm_duty = 254，则占空比为25.4%
    u16 target_pwm_duty = 0;

    // if (LIGHT_CTL_MODE_MUILTY == light_ctl_mode && LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status)
    // {
    //     // 如果当前处于三色切换模式中的关灯状态，不通过旋钮调节灯光亮度，直到触摸切换了灯光颜色
    //     return;
    // }

    // 旋钮开/关引脚为低电平，才能进行后续的adc检测和处理操作
    if (KNOB_ON_OFF_PIN && 1 == last_knob_on_off_pin) // 如果为高电平，直接返回，不检测ad
    {
        return;
    }
    else if (1 == KNOB_ON_OFF_PIN && 0 == last_knob_on_off_pin)
    {
        delay_ms(10); // 旋钮开关会有抖动，这里要延时消抖
        if (!(1 == KNOB_ON_OFF_PIN && 0 == last_knob_on_off_pin))
        {
            return;
        }

        // 如果之前为低电平，现在检测到了高电平，说明要执行关灯操作
        // 关灯后，做好相应的标志位处理，再退出
        last_knob_on_off_pin = KNOB_ON_OFF_PIN; // 更新用于记录的标志位的状态

        // 不管处于什么模式，直接关闭这两个灯
        cur_pwm_duty = 0;
        tmr0_set_pwm_duty(cur_pwm_duty);
        tmr1_set_pwm_duty(cur_pwm_duty);
        yellow_light_close();
        white_light_close();
        light_on_off_status = 0;  // 更新灯光状态
        is_light_lowest_flag = 0; // 清除标志位的状态（灯光关闭之后，不认为它是最小亮度）

        // 下次长按要变成增大亮度（按键长按的相关功能中会打开相应的灯）
        light_adjust_add_sub_flag = 1; // 更新标志位
        send_key_status(0x00);         // 发送不带按键信息的数据，主要是发送当前的灯光状态
        return;
    }
    else if (0 == KNOB_ON_OFF_PIN && 1 == last_knob_on_off_pin)
    {
        delay_ms(10); // 旋钮开关会有抖动，这里要延时消抖
        if (!(0 == KNOB_ON_OFF_PIN && 1 == last_knob_on_off_pin))
        {
            return;
        }
        // 如果之前为高电平，现在为低电平，说明要执行开灯操作，并调节PWM

        if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode ||
            LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status)
        {
            // 如果是白灯
            white_light_open();
        }
        else if (LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status)
        {
            // 如果是黄灯
            yellow_light_open();
        }
        else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
        {
            // 如果是黄白灯
            white_light_open();
            yellow_light_open();
        }
        else if (LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status) // 白灯（从关灯的状态->白灯）
        {
            cur_pwm_duty = cur_white_duty;
            tmr1_set_pwm_duty(cur_pwm_duty); // 白灯
            white_light_open();              // 重新配置IO和定时器

            light_muilty_cur_status = LIGHT_MUILTY_CUR_WHITE;
            // light_on_off_status = 1; // 更新灯的状态
            // send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态（从关灯状态变成白灯，要发送该状态）

            if (cur_pwm_duty <= PWM_MIN_DUTY_LIGHT)
            {
                // 下次长按要变成增大亮度
                light_adjust_add_sub_flag = 1; // 更新用于控制调整灯光亮度的标志位
            }
            else
            {
                // 下次长按要变成减小亮度
                light_adjust_add_sub_flag = 0; // 更新用于控制调整灯光亮度的标志位
            }
        }

        light_on_off_status = 1; // 更新灯光状态
        send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态
    }

    last_knob_on_off_pin = KNOB_ON_OFF_PIN;

    // 如果为低电平，则会来到这里，执行后续的操作
    adc_getval(); // 得到一次滤波后的adc值

    // 下面这里还没有考虑第一次上电的情况，如果第一次上电，旋钮的位置不固定，判断就会有问题
    // 也没有考虑调节到最亮的情况（会导致调不到最亮）
    // 没有考虑到当旋钮处于最小的位置附近的情况
    // if (last_adc_val <= (adc_val + 10) && last_adc_val >= (adc_val - 10)) // 哪怕旋钮没有动作，也会有大于这个Δ的误差
    // if (last_adc_val <= (adc_val + 40) && last_adc_val >= (adc_val - 40)) // 哪怕旋钮没有动作，也会有大于这个Δ的误差
    // if (last_adc_val <= (adc_val + 50) && last_adc_val >= (adc_val - 50)) //
    if ((1 == is_light_lowest_flag && adc_val <= (ADC_MIN_VAL + ADC_STEP_VAL)) ||
        (last_adc_val <= (adc_val + ADC_STEP_VAL) && (last_adc_val >= (adc_val - ADC_STEP_VAL))))
    {
        // 如果测得的数值对应旋钮的最小位置附近，同样退出函数
        // 如果数值在 Δ 以内，则认为没有变化，不进行相应的处理操作
        return;
    }

    last_adc_val = adc_val; //  记录之前检测的adc值

    // 得到当前adc值占总电压值的百分比，精确到小数点后1位
    // （当前检测到的电压值占总电压值的百分比 刚好对应 亮度的百分比）
    target_pwm_duty = (u32)adc_val * 700 / ADC_MAX_VAL;

    // 如果计算的目标占空比超出了当前支持的最大占空比，按最大的占空比处理
    if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode ||
        LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status ||
        LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status)
    {
        if (target_pwm_duty > PWM_MAX_DUTY_SINGLE_LIGHT)
        {
            target_pwm_duty = PWM_MAX_DUTY_SINGLE_LIGHT;
        }
    }
    else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
    {
        if (target_pwm_duty > PWM_MAX_DUTY_MUILTY_LIGHT)
        {
            target_pwm_duty = PWM_MAX_DUTY_MUILTY_LIGHT;
        }
    }

    if (adc_val <= ADC_MIN_VAL) // adc值小于设定的最小值区间，一律按照1.0%的占空比来设置
    // if (target_pwm_duty <= 30) // 如果目标占空比小于3.0%，按照1.0%来设置
    {
        u8 i = 0;
        for (i = 0; i < 10; i++)
        {
            adc_getval();
            if (!(adc_val <= ADC_MIN_VAL))
            {
                is_light_lowest_flag = 0;
                return;
            }
        }

        if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
        {
            target_pwm_duty = PWM_MIN_DUTY_YELLOW_WHITE_LIGHT;
        }
        else
        {
            target_pwm_duty = PWM_MIN_DUTY_LIGHT;
        }
        is_light_lowest_flag = 1;
        last_adc_val = target_pwm_duty * ADC_MAX_VAL / 700 / adc_val;
    }
    else
    {
        is_light_lowest_flag = 0;
    }

#if USE_MY_DEBUG
    printf("测得的adc值 %d\n", adc_val);
    printf("目标占空比 %d\n", target_pwm_duty);
#endif // USE_MY_DEBUG

    while (1)
    {
        target_pwm_duty = __target_pwm_duty_update(target_pwm_duty);

        if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode)
        {
            // 如果当前是单色模式（只控制白灯）
            if (0 == light_on_off_status)
            {
                // 如果没有开灯，则直接退出
                return;
            }

            if (cur_pwm_duty < target_pwm_duty)
            {
                // 如果是要增大亮度

                if (0 == light_on_off_status)
                {
                    // 如果此时灯光是灭的，打开灯光，并更新状态
                    tmr1_set_pwm_duty(cur_pwm_duty);
                    white_light_open();

                    light_on_off_status = 1; // 更新灯光状态
                    send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态
                }

                tmr1_set_pwm_duty(++cur_pwm_duty);
                cur_white_duty = cur_pwm_duty; // 记录白灯的pwm
                // 下次长按要变成减小亮度（按键长按的相关功能中会打开相应的灯）
                light_adjust_add_sub_flag = 0; // 更新标志位
            }
            else if (cur_pwm_duty > target_pwm_duty)
            {
                // 如果是要减小亮度
                tmr1_set_pwm_duty(--cur_pwm_duty);
                cur_white_duty = cur_pwm_duty; // 记录白灯的pwm
                // 下次长按要变成增大亮度（按键长按的相关功能中会打开相应的灯）
                light_adjust_add_sub_flag = 1; // 更新标志位
            }
            else if (cur_pwm_duty == target_pwm_duty)
            {
                // 如果已经到了对应的亮度
#if USE_MY_DEBUG
                printf("当前占空比： %d\n", target_pwm_duty);
#endif
                break; // 退出调节
            }

            light_on_off_status = 1; // 更新灯光状态
        }
        else if (LIGHT_CTL_MODE_MUILTY == light_ctl_mode)
        {
            // 如果当前是三色切换模式
            // 判断当前是不是黄白色，如果是，需要注意最大的占空比为60%，如果不是，最大的占空比为70%
            if (LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status) // 当前为白色灯
            {
                // 如果当前是在增大亮度
                if (cur_pwm_duty < target_pwm_duty)
                {
                    // if (0 == light_on_off_status)
                    // {
                    //     // 如果此时灯光是灭的，打开灯光，并更新状态
                    //     tmr1_set_pwm_duty(cur_pwm_duty);
                    //     white_light_open();

                    //     light_on_off_status = 1; // 更新灯光状态
                    //     send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态
                    // }

                    tmr1_set_pwm_duty(++cur_pwm_duty);
                    cur_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                    // 下次长按要变成减小亮度（按键长按的相关功能中会打开相应的灯）
                    light_adjust_add_sub_flag = 0; // 更新标志位
                }
                else if (cur_pwm_duty > target_pwm_duty)
                {
                    // 如果是要减小亮度
                    tmr1_set_pwm_duty(--cur_pwm_duty);
                    cur_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                    // 下次长按要变成增大亮度（按键长按的相关功能中会打开相应的灯）
                    light_adjust_add_sub_flag = 1; // 更新标志位
                }
                else
                {
                    // 如果已经到了对应的亮度
                    // tmr1_set_pwm_duty(cur_pwm_duty);
                    break;
                }
            }
            else if (LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status) // 如果当前是黄灯
            {
                // 如果当前是在增大亮度
                if (cur_pwm_duty < target_pwm_duty)
                {
                    // if (0 == light_on_off_status)
                    // {
                    //     // 如果此时灯光是灭的，打开灯光，并更新状态
                    //     tmr0_set_pwm_duty(cur_pwm_duty);
                    //     yellow_light_open();

                    //     light_on_off_status = 1; // 更新灯光状态
                    //     send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态
                    // }

                    tmr0_set_pwm_duty(++cur_pwm_duty);
                    cur_yellow_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                    // 下次长按要变成减小亮度（按键长按的相关功能中会打开相应的灯）
                    light_adjust_add_sub_flag = 0; // 更新标志位
                }
                else if (cur_pwm_duty > target_pwm_duty)
                {
                    // 如果是要减小亮度
                    tmr0_set_pwm_duty(--cur_pwm_duty);
                    cur_yellow_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                    // 下次长按要变成增大亮度（按键长按的相关功能中会打开相应的灯）
                    light_adjust_add_sub_flag = 1; // 更新标志位
                }
                else
                {
                    // 如果已经到了设定的亮度
                    // tmr0_set_pwm_duty(cur_pwm_duty);
                    break;
                }
            }
            else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
            {
                // 如果当前是黄白色

                // 如果当前是在增大亮度
                if (cur_pwm_duty < target_pwm_duty)
                {
                    // if (0 == light_on_off_status)
                    // {
                    //     // 如果此时灯光是灭的，打开灯光，并更新状态
                    //     tmr0_set_pwm_duty(cur_pwm_duty);
                    //     tmr1_set_pwm_duty(cur_pwm_duty);
                    //     yellow_light_open();
                    //     white_light_open();

                    //     light_on_off_status = 1; // 更新灯光状态
                    //     send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态
                    // }

                    tmr0_set_pwm_duty(++cur_pwm_duty);
                    tmr1_set_pwm_duty(cur_pwm_duty);
                    cur_yellow_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                    // 下次长按要变成减小亮度（按键长按的相关功能中会打开相应的灯）
                    light_adjust_add_sub_flag = 0; // 更新标志位
                }
                else if (cur_pwm_duty > target_pwm_duty)
                {
                    // 如果当前是在减小亮度
                    tmr0_set_pwm_duty(--cur_pwm_duty);
                    tmr1_set_pwm_duty(cur_pwm_duty);

                    cur_yellow_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                    // 下次长按要变成增大亮度（按键长按的相关功能中会打开相应的灯）
                    light_adjust_add_sub_flag = 1; // 更新标志位
                }
                else
                {
                    // 如果已经到了最大亮度
                    break;
                }
            }
            else
            {
                break; // 如果不是白色灯光、黄色灯光和黄白色灯光，直接返回
            }
        } // 如果当前是三色切换模式

        // delay_100us(15); // 每1.5ms调整0.1%的占空比
        delay_100us(PWM_ADJUST_DUTY_TIME_STEP); // 每x.xms调整0.1%的占空比
    } // while(1)
}

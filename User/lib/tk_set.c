// encoding GB2312
/**
 ******************************************************************************
 * @file    User/tk_lib.c
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    05-20-2022
 * @brief   Main program body
 ******************************************************************************
 * @attention
 * tk_set.c文件是系统使用的文件，不建议修改。
 * 以下函数和变量用户不需要修改
 *
 *
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include "my_config.h"

/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @}
 */
// 配置一个10ms的中断
#define PEROID_VAL (SYSCLK / 128 / 100 - 1)
// 用户不允许修改
volatile unsigned short xdata __tk_ch_data_0[TK_CH_USE] _at_(0x6000 + 0);
volatile unsigned short xdata __tk_ch_data_1[TK_CH_USE] _at_(0x6000 + TK_CH_USE * 2);
volatile unsigned short xdata __tk_ch_data_2[TK_CH_USE] _at_(0x6000 + TK_CH_USE * 4);
volatile unsigned short xdata __tk_ch_data_3[TK_CH_USE] _at_(0x6000 + TK_CH_USE * 8);
volatile unsigned short xdata __tk_ch_data_4[TK_CH_USE] _at_(0x6000 + TK_CH_USE * 10);
volatile unsigned short xdata __tk_ch_data_5[TK_CH_USE];
volatile unsigned short xdata __tk_ch_data_6[TK_CH_USE];
volatile unsigned short xdata __tk_ch_fth[TK_CH_USE];
volatile unsigned short xdata __tk_i_set[TK_CH_USE];
volatile unsigned char xdata __tk_update_cnt[TK_CH_USE];
volatile unsigned char xdata __tk_confirm_cnt[TK_CH_USE];
volatile unsigned char xdata __tk_leave_cnt[TK_CH_USE];
volatile unsigned char xdata __tk_ch_index[TK_CH_USE];

// 用户不允许修改
unsigned long code __tk_adjust_line = TK_DATA_LINE;
unsigned short code __tk_adjust_time = TK_ADJUST_TIME;
unsigned short code __tk_adjust_diff_valu = TK_MAX_DIFF_VALU;
unsigned char code __tk_adjust_en = TK_ADJUST_EN;

unsigned short code __tk_valid_time = TK_VALID_TIME;
unsigned short code __tk_long_key_time = TK_LONG_KEY_TIME;
unsigned short code __tk_noise_value = TK_NOISE_VAL;
unsigned char code __tk_cs_en = TK_CS_EN;
unsigned char code __tk_tp_en = TK_TP_EN;
unsigned char code __tk_nm_num = TK_MU_CNT;
unsigned char code __tk_base_update_cnt = TK_UPDATE_CNT;
unsigned char code __tk_nm_cm_value = TK_NM_CM_CNT;
unsigned char code __tk_cm_valu = TK_CM_VALE;
unsigned char code __tk_use_num = TK_CH_USE;

/*
**触摸通道信息表
*/
static u16 code TK_CH_EN_BUG[][2] =
    {
#if TK0_CH_EN
        {
            0u,           // 通道值
            TK0_THR_DATA, // 门限值
        },
#endif

#if TK1_CH_EN
        {
            1u,
            TK1_THR_DATA,
        },
#endif

#if TK2_CH_EN
        {
            2u,
            TK2_THR_DATA,
        },
#endif

#if TK3_CH_EN
        {
            3u,
            TK3_THR_DATA,
        },
#endif

#if TK4_CH_EN
        {
            4u,
            TK4_THR_DATA,
        },
#endif

#if TK5_CH_EN
        {
            5u,
            TK5_THR_DATA,
        },
#endif

#if TK6_CH_EN
        {
            6u,
            TK6_THR_DATA,
        },
#endif

#if TK7_CH_EN
        {
            7u,
            TK7_THR_DATA,
        },
#endif

#if TK8_CH_EN
        {
            8u,
            TK8_THR_DATA,
        },
#endif

#if TK9_CH_EN
        {
            9u,
            TK9_THR_DATA,
        },
#endif

#if TK10_CH_EN
        {
            10u,
            TK10_THR_DATA,
        },
#endif

#if TK11_CH_EN
        {
            11u,
            TK11_THR_DATA,
        },
#endif

#if TK12_CH_EN
        {
            12u,
            TK12_THR_DATA,
        },
#endif

#if TK13_CH_EN
        {
            13u,
            TK13_THR_DATA,
        },
#endif

#if TK14_CH_EN
        {
            14u,
            TK14_THR_DATA,
        },
#endif

#if TK15_CH_EN
        {
            15u,
            TK15_THR_DATA,
        },
#endif

#if TK16_CH_EN
        {
            16u,
            TK16_THR_DATA,
        },
#endif

#if TK17_CH_EN
        {
            17u,
            TK17_THR_DATA,
        },
#endif

#if TK18_CH_EN
        {
            18u,
            TK18_THR_DATA,
        },
#endif

#if TK19_CH_EN
        {
            19u,
            TK19_THR_DATA,
        },
#endif

#if TK20_CH_EN
        {
            20u,
            TK20_THR_DATA,
        },
#endif

#if TK21_CH_EN
        {
            21u,
            TK21_THR_DATA,
        },
#endif

#if TK22_CH_EN
        {
            22u,
            TK22_THR_DATA,
        },
#endif

#if TK23_CH_EN
        {
            23u,
            TK23_THR_DATA,
        },
#endif

#if TK24_CH_EN
        {
            24u,
            TK24_THR_DATA,
        },
#endif

#if TK25_CH_EN
        {
            25u,
            TK25_THR_DATA,
        },
#endif
};

/**
 * @brief  Touchkey gpio init function
 * @param  None
 * @retval None
 */
void tk_gpio_config(void)
{
    u8 i = 0;

    for (i = 0; i < TK_CH_USE; i++)
    {

        if (__tk_ch_index[i] < 8)
        {
            if (__tk_ch_index[i] < 4)
            {
                P0_MD0 &= ~(0x3 << (__tk_ch_index[i] - 0) * 2);
                P0_MD0 |= (0x3 << (__tk_ch_index[i] - 0) * 2);
            }
            else
            {
                P0_MD1 &= ~(0x3 << (__tk_ch_index[i] - 4) * 2);
                P0_MD1 |= (0x3 << (__tk_ch_index[i] - 4) * 2);
            }
        }
        else if ((__tk_ch_index[i] >= 8) && (__tk_ch_index[i] < 16))
        {
            if (__tk_ch_index[i] < 12)
            {
                P1_MD0 &= ~(0x3 << (__tk_ch_index[i] - 8) * 2);
                P1_MD0 |= (0x3 << (__tk_ch_index[i] - 8) * 2);
            }
            else
            {
                P1_MD1 &= ~(0x3 << (__tk_ch_index[i] - 12) * 2);
                P1_MD1 |= (0x3 << (__tk_ch_index[i] - 12) * 2);
            }
        }
        else
        {
            if (__tk_ch_index[i] < 20)
            {
                P2_MD0 &= ~(0x3 << (__tk_ch_index[i] - 16) * 2);
                P2_MD0 |= (0x3 << (__tk_ch_index[i] - 16) * 2);
            }
            else
            {
                P2_MD1 &= ~(0x3 << (__tk_ch_index[i] - 20) * 2);
                P2_MD1 |= (0x3 << (__tk_ch_index[i] - 20) * 2);
            }
        }
    }
}

/**
 * @brief  Touchkey_IRQHandler
 * @param  none
 * @retval None
 */
void TK_IRQHandler(void) interrupt TK_IRQn
{
    __IRQnIPnPush(TK_IRQn);
    if (TK_CON2 & (0x1 << 6))
    {
        TK_CON2 |= (0x1 << 6);
        __tk_handler();
    }
    __IRQnIPnPop(TK_IRQn);
}

/**
 * @brief  Touchkey Module init function
 * @param  None
 * @retval None
 */
void tk_init(void)
{
    IE_EA = 1;
    IE3 |= (0x1 << 3);
    IP6 |= (0x3 << 6);
    CLK_CON2 |= (0x1 << 6);
    __EnableIRQ(TK_IRQn);
    TK_ACON0 = 0x3F;
    TK_ACON1 = 0x4B;
    TK_ACON3 = 0x30;
    TK_PSRCNT = 0x2F;
    TK_APRECHARGE = 0x4f;
    TK_APREDISCH = 0x27;
    TK_ACONVTIME = 0xA8;
    TK_BASEDIV0 = 0x10;
    TK_BASEDIV1 = 0x0;
    TK_BASEDIV2 = 0x0;
    TK_BASEDIV3 = 0x0;
    TK_CHCON3 = 0xD0;
    TK_CON0 = 0x4;
    TK_CON1 = 0x1E;
    TK_CON2 = 0x02;
}

/**
 * @brief  TIMR0_IRQHandler function
 * @param  None
 * @retval None
 */
void WUT_IRQHandler(void) interrupt WUT_IRQn
{
    // 进入中断设置IP，不可删除
    __IRQnIPnPush(WUT_IRQn);
    // 周期中断
    if (WUT_CONH & TMR_PRD_PND(0x1))
    {
        WUT_CONH |= TMR_PRD_PND(0x1); // 清除pending
        __tk_ms_handler();            // 闭源的，不知道做了什么，看上去是让CPU进入睡眠，单位为ms（手册上说是睡眠300ms）
    }

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(WUT_IRQn);
}

#if 0
/**
 * @brief  TIMER Module init function
 * @param  None
 * @retval None
 */
void wut_init(void)
{
    __EnableIRQ(WUT_IRQn);
    IE_EA = 1;

    // 设置timer2的计数功能，配置一个10ms的中断
    TMR_ALLCON = WUT_CNT_CLR(0x1);
    WUT_PRH = TMR_PERIOD_VAL_H((PEROID_VAL >> 8) & 0xFF);                       // timer2计数周期高8位
    WUT_PRL = TMR_PERIOD_VAL_L((PEROID_VAL >> 0) & 0xFF);                       // timer2计数周期低8位
    WUT_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                          // 清除中断标志位并使能中断
    WUT_CONL = TMR_SOURCE_SEL(0x7) | TMR_PRESCALE_SEL(0x7) | TMR_MODE_SEL(0x1); // 配置timer2时钟源、预分频值、模式
}
#endif // end void wut_init(void)

/**
 * @brief  Touchkey  parameter configuration function
 *         按键参数配置函数
 *
 * @param  None
 * @retval None
 */
void tk_param_init(void)
{
    u8 i = 0;

    /* 按键通道、电流、灵敏度初始化 */
    for (i = 0; i < TK_CH_USE; i++)
    {
        __tk_ch_index[i] = (u8)(TK_CH_EN_BUG[i][0] & 0xff); // 存放按键通道信息
        __tk_ch_fth[i] = TK_CH_EN_BUG[i][1];                // 存放按键通道信息
        __tk_i_set[i] = TK_CURR_GEAR;                       // 充电电流
        __tk_ch_en |= (1UL << __tk_ch_index[i]);            // 按键通道使能
    }

    /* 按键IO配置函数 */
    tk_gpio_config();

    /* 库函数初始化 */
    __tk_lib_init(); // 调用了库（闭源）

    /* 按键模块配置函数 */
    tk_init();

    /* 定时器配置函数 */
    // wut_init();
}

/**
 * @brief  Touchkey  Circular execution function
 * @param  None
 * @retval None
 */
void tk_handle(void)
{
    /* 按键初始化 */
    tk_param_init();

    /* 用户代码初始化接口 */
    user_init();

    check_light_ctl_mode();   // 上电后先检测要使用哪种灯光控制模式，最后，会配置驱动黄灯的PWM
    white_light_pwm_config(); // 配置驱动白灯的PWM

#if USE_MY_DEBUG
    // light_ctl_mode = LIGHT_CTL_MODE_MUILTY; // 接下来触摸按键不仅可以控制开关灯，还可以控制三种颜色灯光的切换（light_ctl_mode = 0）
    light_ctl_mode = LIGHT_CTL_MODE_SINGLE;
#endif // USE_MY_DEBUG

    // light_ctl_mode = LIGHT_CTL_MODE_SINGLE; // 测试用
    // light_ctl_mode = LIGHT_CTL_MODE_MUILTY; // 测试用
    // light_control_flag = LIGHT_WHITE_CLOSE;

    if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode)
    {
        tmr2_enable(); // 单色调光模式下，才使用定时器2来实现开关灯的亮度渐变
    }

    /* 系统主循环 */
    while (1)
    {
        // 上电2000ms后再使能，防止一上电就发送数据
        if (0 == touch_ctl_enable)
        {
            /* 按键扫描函数 */
            __tk_scan(); // 使用了库里面的接口（闭源库）

            delay_ms(1);
            touch_ctl_enable_time_cnt++;
            if (touch_ctl_enable_time_cnt >= 2000)
            {
                touch_ctl_enable = 1; // 使能触摸按键的功能
                __tk_key_flag = 0; // 使能后，清除之前扫描到的按键，一律作废
            }
        }
        else
        {
            /* 按键扫描函数 */
            __tk_scan(); // 使用了库里面的接口（闭源库）

            /* 用户循环扫描函数接口 */
            user_handle();
        }

        knob_handle();

#if 1
        if (is_touch_key_light_press) // 如果按键--灯光按下（单击）
        {
            is_touch_key_light_press = 0;

            // 如果是单色模式，只控制白灯
            if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode)
            {
                if (light_on_off_status)
                {
                    // 如果灯光已经开启
                    // |-->关灯
                    light_control_flag = LIGHT_WHITE_CLOSE;

#if USE_MY_DEBUG
                    P24 = ~P24;
#endif // #if USE_MY_DEBUG

                    light_on_off_status = 0; // 更新灯的状态
                    send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态

                    // 下次长按要变成增大亮度
                    light_adjust_add_sub_flag = 1; // 更新用于控制调整灯光亮度的标志位
                }
                else
                {
                    // 如果灯光已经关闭
                    // |-->开灯
                    light_control_flag = LIGHT_WHITE_OPEN;

#if USE_MY_DEBUG
                    P24 = ~P24;
#endif // #if USE_MY_DEBUG

                    light_on_off_status = 1; // 更新灯的状态
                    send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态

                    if (cur_white_duty == PWM_MIN_DUTY_LIGHT)
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
            }
            else // 如果是三色切换模式，每次触摸到按键，在白->黄->黄白->关闭->白中循环切换
            {
                if (LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status) // 白灯（从关灯的状态->白灯）
                {
                    cur_pwm_duty = cur_white_duty;
                    tmr1_set_pwm_duty(cur_pwm_duty); // 白灯
                    white_light_open();              // 重新配置IO和定时器

                    light_muilty_cur_status = LIGHT_MUILTY_CUR_WHITE;
                    light_on_off_status = 1; // 更新灯的状态
                    send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态（从关灯状态变成白灯，要发送该状态）

                    if (cur_pwm_duty == PWM_MIN_DUTY_LIGHT)
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
                else if (LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status) // 黄灯（从白灯->黄灯）
                {
                    cur_pwm_duty = cur_yellow_duty;

                    tmr1_set_pwm_duty(0); // 关闭白灯
                    white_light_close();  // 重新配置IO和定时器

                    tmr0_set_pwm_duty(cur_pwm_duty); // 黄灯
                    yellow_light_open();             // 重新配置IO和定时器

                    light_muilty_cur_status = LIGHT_MUILTY_CUR_YELLOW;
                    light_on_off_status = 1; // 更新灯的状态
                    send_key_status(0x00);   // 发送不带按键信息的数据

                    if (cur_pwm_duty == PWM_MIN_DUTY_LIGHT)
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
                else if (LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status) // 黄白灯，需要注意最大亮度的占空比为60%（从黄灯->黄白灯）
                {
                    cur_pwm_duty = cur_yellow_white_duty;
                    tmr0_set_pwm_duty(cur_pwm_duty);
                    tmr1_set_pwm_duty(cur_pwm_duty);

                    white_light_open(); // 之前关闭了白灯，现在要重新配置IO和定时器

                    light_muilty_cur_status = LIGHT_MUILTY_CUR_YELLOW_WHITE;
                    light_on_off_status = 1; // 更新灯的状态
                    send_key_status(0x00);   // 发送不带按键信息的数据

                    if (cur_pwm_duty == PWM_MIN_DUTY_LIGHT)
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
                else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status) // 关灯（从黄白灯->关灯）
                {
                    cur_pwm_duty = 0;
                    tmr0_set_pwm_duty(cur_pwm_duty);
                    tmr1_set_pwm_duty(cur_pwm_duty);
                    white_light_close();
                    yellow_light_close();

                    light_muilty_cur_status = LIGHT_MUILTY_CUR_OFF;
                    light_on_off_status = 0; // 更新灯的状态
                    send_key_status(0x00);   // 发送不带按键信息的数据，主要是发送当前的灯光状态

                    // // 下次长按要变成减小亮度
                    // light_adjust_add_sub_flag = 0; // 更新用于控制调整灯光亮度的标志位
                }
            }
        }
#endif

#if 0  // 在demo板上进行开灯和关灯的功能测试--测试通过
        white_light_close();
        delay_ms(500);
        white_light_open();
        delay_ms(500);
#endif // 在demo板上进行开灯和关灯的功能测试--测试通过

        /* 喂狗 :建议不要关闭看门狗，默认2s复位*/
        // WDT_KEY = WDT_KEY_VAL(0xAA);
    }
}

/*************************** (C) COPYRIGHT 2022 TAIXIN-IC ***** END OF FILE *****/

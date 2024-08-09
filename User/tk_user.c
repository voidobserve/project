/**
 ******************************************************************************
 * @file    User/tk_user.c
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    05-20-2022
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2022 TAIXIN-IC</center></h2>
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

#ifdef DEVELOPMENT_BOARD
/**
 * @brief  Touchkey  debug uart init function
 * @param  None
 * @retval None
 */
xdata u16 led_data[1];                           // LED显示buff
#define LED_DIS_FREQ (1000000 / 16 / 8 / 60 - 1) // 一个CON的显示时间60Hz    60-1=1M/(LED_COMCON*16*LED_TIME_CON)

unsigned int xdata display_data = 0;
#endif // #ifdef DEVELOPMENT_BOARD

/**
 * @brief  调试接口IO初始化函数.
 * @param  None
 * @retval None
 */
#if TK_DEBUG_EN
void debug_gpio_config(void)
{
#if 1
    P2_MD0 &= ~GPIO_P21_MODE_SEL(0x03);
    P2_MD0 |= GPIO_P21_MODE_SEL(0x01);
    FOUT_S21 = GPIO_FOUT_UART1_TX;
#else
    P2_MD1 &= ~GPIO_P27_MODE_SEL(0x03);
    P2_MD1 |= GPIO_P27_MODE_SEL(0x01);
    FOUT_S27 = GPIO_FOUT_UART1_TX;
#endif
}
#endif

#ifdef DEVELOPMENT_BOARD
/**
 * @brief  LED INIT FUNCTION
 * @param  None
 * @retval None
 */
// 在开发板上：
// P2_7连接到四个蓝色LED的负极的公共端
// P0_3连接到LED1正极
// P3_0连接到LED2正极
// P1_0连接到LED3正极
// P0_7连接到LED4正极
void led_init(void)
{
    // 配置COM0   P27为输出模式
    // P2_MD1，P2的工作模式寄存器1
    P2_MD1 &= ~GPIO_P27_MODE_SEL(0x3); // 清零P2_MD1寄存器的6-7位
    P2_MD1 |= GPIO_P27_MODE_SEL(0x1);  // P2_MD1的6~7位设置为0x01，这样会将P2的第7个IO设置为输出模式

    // 配置IO的驱动能力
    // P2的第7个IO的驱动电流为8mA
    P2_DRV7 = GPIO_P27_DRV_SEL(0x1);

    // 配置SEG0-SEG4 P03\P30\P10\P07-输出模式
    // P0_3、P3_0、P1_0和P0_1配置为输出模式
    // 下面是先清零，再置位的操作
    P0_MD0 &= ~GPIO_P03_MODE_SEL(0x3);
    P3_MD0 &= ~GPIO_P30_MODE_SEL(0x3);
    P1_MD0 &= ~GPIO_P10_MODE_SEL(0x3);
    P0_MD1 &= ~GPIO_P07_MODE_SEL(0x3);

    P0_MD0 |= GPIO_P03_MODE_SEL(0x1);
    P3_MD0 |= GPIO_P30_MODE_SEL(0x1);
    P1_MD0 |= GPIO_P10_MODE_SEL(0x1);
    P0_MD1 |= GPIO_P07_MODE_SEL(0x1);

    // SEG0-SEG4 P03\P30\P10\P07
    // 配置P0_3、P3_0、P1_0和P0_1的输出功能，驱动LED（应该是连接到了片上外设LED模块，这个模块是个控制器）
    FOUT_S03 = GPIO_FOUT_LED_SEG0;
    FOUT_S30 = GPIO_FOUT_LED_SEG1;
    FOUT_S10 = GPIO_FOUT_LED_SEG2;
    FOUT_S07 = GPIO_FOUT_LED_SEG3;

    // 配置P0_3、P3_0、P1_0和P0_1的输出电流，8mA
    P0_DRV3 = GPIO_P03_DRV_SEL(0x1);
    P3_DRV0 = GPIO_P30_DRV_SEL(0x1);
    P1_DRV0 = GPIO_P10_DRV_SEL(0x1);
    P0_DRV7 = GPIO_P07_DRV_SEL(0x1);

    // COM0 P27
    // 配置P2_7的输出功能，连接到LED的公共端0
    FOUT_S27 = GPIO_FOUT_LED_COM0;

    CLK_CON2 |= CLK_LED_EN(0x1); // 打开LED模块的时钟（打开片上外设LED模块的时钟）

    // 0x55表示允许访问和设置 wdt_psr（在WDT_CON中，psr表示看门狗定时时间）
    WDT_KEY = 0x55;                // 操作IO_MAP寄存器需要写KEY
    IO_MAP |= MAP_LED_DMA_EN(0x1); // LED DMA使能
    WDT_KEY = 0xBB;                // 写入一个错误的数值，恢复该寄存器的写保护

    // 打开LED模块的时钟
    CLK_CON2 |= 0x10;

    LED_SEGCONL = LED_SEG0_EN(0x1) | // SEG0 使能
                  LED_SEG1_EN(0x1) | // SEG0 使能
                  LED_SEG2_EN(0x1) | // SEG0 使能
                  LED_SEG3_EN(0x1);  // SEG0 使能
    LED_COMCON = LED_COM0_EN(0x1);   // COM1 使能

    LED_DMAADRH = LED_DMA_ADDR_H((((u16)led_data) >> 8) & 0xFF); // LED数据储存首地址的高8位
    LED_DMAADRL = LED_DMA_ADDR_L((((u16)led_data) >> 0) & 0xFF); // LED数据储存首地址的低8位
    LED_TIMECON = LED_SCAN_TIME(LED_DIS_FREQ);                   // 扫描到某个com或seg时点亮的时间,步长为32微秒
    LED_CON = LED_COM_SEG_SEL(0x0) |                             // 扫描方式选择com扫描
              LED_EN(0x1);                                       // LED使能,使能之后从DMAADDRH和DMAADDRL的地址开始拿数据扫描点亮,用户将数据写到对应的地址即可

    led_data[0] = 0x00000f00; // 一开始让四个LED全部点亮
    // led_data[0] = 0x00000000; // 一开始让四个LED全部熄灭
}

void led_display(void)
{
    led_data[0] = display_data;
}
#endif // #ifdef DEVELOPMENT_BOARD

/**
 * @brief  用户代码初始化函数接口.
 * @param  None
 * @retval None
 */
void user_init(void)
{
    // led_init(); // 初始化开发板LED相关的引脚

#ifdef DEVELOPMENT_BOARD // demo板上使用串口0来作为调试
    uart0_config();      // 调试用到的串口
    p24_output_config(); // 调试用的，观察引脚的电平变化
#endif                   // demo板上使用串口0来作为调试

    uart1_config(); // 单线通信的配置
    tmr2_config();  // 定时器2的配置，配置完成后，默认是关闭的
    knob_config();  // 旋钮的配置
    // tmr2_enable();

#if USE_MY_DEBUG
    // p25_output_config(); //
#endif // USE_MY_DEBUG
}

#ifdef DEVELOPMENT_BOARD // 开发板上对应额触摸按键函数接口
/**
 * @brief  用户代码循环执行函数接口.（触摸按键扫描函数接口）
 * @param  None
 * @retval None
 */
void user_handle(void)
{
    volatile unsigned long int KeyOnOutput = __tk_key_flag; // __tk_key_flag单次按键标志
    static volatile unsigned long int KeyOld = 0;           // 用于存放长按时，对应的按键

    // 是否长按的标志，0--否，1--是
    // 需要保存长按的状态，表示定时器一直在发送键值
    static unsigned char long_touch_flag = 0; // 上一次扫描到的按键是否为长按的标志
    static volatile u32 cnt = 0;              // 长按计数值

    u32 i = 0; // 循环计数值

    if (KeyOnOutput && 0 == long_touch_flag && cnt < TK_LONG_KEY_TIME) // 如果有按键按下（并且之前扫描到的不是长按）
    {
#if 1
        // 通过循环判断是否为长按
        for (i = 0; i < 500000; i++) // 实际上用不到这么大的数字，到了设定的长按时间就会退出
        {
            // 按键扫描函数，使用了库里面的接口（闭源库）
            // 这个函数会更新__tk_key_flag的值
            delay_ms(10);
            __tk_scan();

            if (KeyOnOutput == __tk_key_flag)
            {
                // 如果按键键值一致，说明按键一直被按下，未松开
                cnt++;

                if (TK_CH0_VALIB == KeyOnOutput)
                {
                    printf("检测到按键1按下，并且还处于是否长按的判定中\n");
                }
                else
                {
                    // 如果是其他按键，不用检测长按
                    break;
                }
            }
            else
            {
                // 如果按键键值不一致或是松开了按键
                break;
            }
            if (cnt > TK_LONG_KEY_TIME) // 如果长按超过了这里设置的时间
            {
                long_touch_flag = 1; // 标记为长按
                break;
            }
        }
#endif

        if (cnt > TK_LONG_KEY_TIME)
        {
            long_touch_flag = 1; // 标记为长按
        }
        else
        {
            // 如果是短按
            cnt = 0; // 清除长按的计数值

            // 判断当前触摸的键值，做对应的操作
            if (TK_CH0_VALIB == KeyOnOutput)
            {

                // 等待手指松开按键，不然发送完带有双击状态的键值后会再发送一次单击状态的键值
                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // 按键不松开，就一直发送数据
                    //  send_key_status(TOUCH_KEY_BULETOOTH);
                    printf("按键1按下\n");
                }

                // 按键松开时，再提示按键按下的信息
                printf("按键1按下并松开\n");
                is_touch_key_light_press = 1;

                return;
            }
            else if (TK_CH4_VALIB == KeyOnOutput)
            {
                // 如果开发板的TK2被触摸
                // display_data ^= 0x0200; // 对应的LED状态取反，后面会调用led_display()刷新
                for (i = 0; i < 300; i++)
                {
                    delay_ms(1);
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;

                    if (TK_CH4_VALIB == KeyOnOutput)
                    {
                        // 如果检测到是同一个按键按下，说明是双击
                        // 等待手指松开按键，不然发送完带有双击状态的键值后会再发送一次单击状态的键值
                        while (0 != KeyOnOutput)
                        {
                            __tk_scan();
                            KeyOnOutput = __tk_key_flag;
                        }

                        // send_status_keyval(KEY_PRESS_DOUBLECLICK, KEY_TK2);
                        return;
                    }
                }

                // 如果不是同一个按键按下或是没有值，说明是短按
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK2); // 发送带有状态的键值
            }
            else if (TK_CH5_VALIB == KeyOnOutput)
            {
                // 如果开发板的TK3被触摸
                // display_data ^= 0x0400; // 对应的LED状态取反，后面会调用led_display()刷新
                for (i = 0; i < 300; i++)
                {
                    delay_ms(1);
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;

                    if (TK_CH5_VALIB == KeyOnOutput)
                    {
                        // 如果检测到是同一个按键按下，说明是双击
                        // 等待手指松开按键，不然发送完带有双击状态的键值后会再发送一次单击状态的键值
                        while (0 != KeyOnOutput)
                        {
                            __tk_scan();
                            KeyOnOutput = __tk_key_flag;
                        }

                        // send_status_keyval(KEY_PRESS_DOUBLECLICK, KEY_TK3);
                        return;
                    }
                }

                // 如果不是同一个按键按下或是没有值，说明是短按
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK3); // 发送带有状态的键值
            }
            else if (TK_CH6_VALIB == KeyOnOutput)
            {
                // 如果开发板的TK4被触摸
                // display_data ^= 0x0800; // 对应的LED状态取反，后面会调用led_display()刷新
                for (i = 0; i < 300; i++)
                {
                    delay_ms(1);
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;

                    if (TK_CH6_VALIB == KeyOnOutput)
                    {
                        // 如果检测到是同一个按键按下，说明是双击
                        // 等待手指松开按键，不然发送完带有双击状态的键值后会再发送一次单击状态的键值
                        while (0 != KeyOnOutput)
                        {
                            __tk_scan();
                            KeyOnOutput = __tk_key_flag;
                        }

                        // send_status_keyval(KEY_PRESS_DOUBLECLICK, KEY_TK4);
                        return;
                    }
                }

                // 如果不是同一个按键按下或是没有值，说明是短按
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK4); // 发送带有状态的键值
            }
        }
    }

#if 1
    if (1 == long_touch_flag && 0 != KeyOnOutput)
    {
        KeyOld = KeyOnOutput;

        // 如果之前是长按，现在还未松开
        // 判断当前触摸的键值，做对应的操作
        if (TK_CH0_VALIB == KeyOnOutput)
        {
            // 如果开发板的TK1被触摸
            if (cnt >= TK_LONG_KEY_TIME && cnt < 90)
            {
                printf("按键1长按\n");
            }

#if USE_MY_DEBUG
            P24 = ~P24;
#endif // USE_MY_DEBUG

            // 若还不松开手
            while (0 != KeyOnOutput)
            {
                __tk_scan();
                KeyOnOutput = __tk_key_flag;

                // 进行灯光调节
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
#if USE_MY_DEBUG
                            P24 = ~P24;
#endif // USE_MY_DEBUG
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
                            // cur_pwm_duty = 0;
                            tmr1_set_pwm_duty(cur_pwm_duty);
                            white_light_close();

                            light_on_off_status = 0; // 更新灯光状态
#if USE_MY_DEBUG
                            P24 = ~P24;
#endif // USE_MY_DEBUG
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
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
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
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
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
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
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
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
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
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
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
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status)
                    {
                        // 如果当前灯是关闭的，松开手时，把灯打开
                        is_touch_key_light_press = 1;
                        light_adjust_add_sub_flag = 1;
                        // return;
                    }
                }

                // 在这里发送按键数据
                delay_100us(15); // 每1.5ms调整0.1%的占空比
            }

            // 如果长按后松手了，要更新当前是在增大亮度还是减少亮度的标志位
            if (light_adjust_add_sub_flag)
            {
                // 如果之前是在增大亮度，下次长按要变成减小亮度
                light_adjust_add_sub_flag = 0; // 更新标志位
            }
            else
            {
                // 如果之前是在减小亮度，下次长按要变成增大亮度
                light_adjust_add_sub_flag = 1; // 更新标志位
            }
        }
    }
    else if (1 == long_touch_flag && 0 == KeyOnOutput)
    {
        // 如果之前是长按，现在却松开了按键
        switch (KeyOld)
        {
        case TK_CH0_VALIB:
            printf("按键1长按后松开了按键\n");
            // send_key_status(1, 0);
            break;

        case TK_CH4_VALIB:
            // send_status_keyval(KEY_PRESS_LOOSE, KEY_TK2);
            break;

        case TK_CH5_VALIB:
            // send_status_keyval(KEY_PRESS_LOOSE, KEY_TK3);
            break;

        case TK_CH6_VALIB:
            // send_status_keyval(KEY_PRESS_LOOSE, KEY_TK4);
            break;
        }

        long_touch_flag = 0; // 清除标志位
        cnt = 0;
        KeyOld = 0;
    }
#endif

    // led_display(); // LED状态更新显示
}
#endif // end ifdef DEVELOPMENT_BOARD

#ifdef CIRCUIT_BOARD // 目标电路板对应的触摸按键函数接口
/**
 * @brief  用户代码循环执行函数接口.（触摸按键扫描函数接口）
 * @param  None
 * @retval None
 */
void user_handle(void)
{
    volatile unsigned long int KeyOnOutput = __tk_key_flag; // __tk_key_flag单次按键标志
    static volatile unsigned long int KeyOld = 0;           // 用于存放长按时，对应的按键

    // 是否长按的标志，0--否，1--是
    // 需要保存长按的状态，表示定时器一直在发送键值
    static unsigned char long_touch_flag = 0; // 上一次扫描到的按键是否为长按的标志
    static volatile u32 cnt = 0;              // 长按计数值

    u32 i = 0; // 循环计数值

    if (KeyOnOutput && 0 == long_touch_flag && cnt < TK_LONG_KEY_TIME) // 如果有按键按下（并且之前扫描到的不是长按）
    {
        // 通过循环判断是否为长按
        for (i = 0; i < 500000; i++) // 实际上用不到这么大的数字，到了设定的长按时间就会退出
        {
            // 按键扫描函数，使用了库里面的接口（闭源库）
            // 这个函数会更新__tk_key_flag的值
            delay_ms(10);
            __tk_scan();

            if (KeyOnOutput == __tk_key_flag)
            {
                // 如果按键键值一致，说明按键一直被按下，未松开
                cnt++;

                // 如果是灯光按键按下，继续发送按键的状态
                if (TK_CH2_VALIB == KeyOnOutput)
                {
                    send_key_status(TOUCH_KEY_LIGHT);
                }
                else
                {
                    // 如果是其他按键，不用检测长按
                    break;
                }
            }
            else
            {
                // 如果按键键值不一致或是松开了按键
                // cnt = 0;
                break;
            }

            if (cnt > TK_LONG_KEY_TIME) // 如果长按超过了这里设置的时间
            {
                long_touch_flag = 1; // 标记为长按
                break;
            }
        }

        if (cnt > TK_LONG_KEY_TIME)
        {
            long_touch_flag = 1; // 标记为长按
        }
        else
        {
            // 如果是短按
            cnt = 0; // 清除长按的计数值

            // 判断当前触摸的键值，做对应的操作
            if (TK_CH3_VALIB == KeyOnOutput)
            {
                // 如果触摸了按键--蓝牙

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // 按键不松开，就一直发送数据
                    send_key_status(TOUCH_KEY_BULETOOTH);
                }

                // 按键松开，发送对应信息
                // uart1_sendbyte(0x10);
                // uart1_sendbyte(~0x10);
                return;
            }
            else if (TK_CH2_VALIB == KeyOnOutput)
            {
                // 如果按键--灯被触摸

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // 按键不松开，就一直发送数据
                    send_key_status(TOUCH_KEY_LIGHT);
                }

                // 按键松开，发送对应信息
                // uart1_sendbyte(0x20);
                // uart1_sendbyte(~0x20);

                is_touch_key_light_press = 1; // 标志位置一
                return;
            }
            else if (TK_CH0_VALIB == KeyOnOutput)
            {
                // 如果按键--时钟被触摸

                // 如果不是同一个按键按下或是没有值，说明是短按
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK3); // 发送带有状态的键值

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // 按键不松开，就一直发送数据
                    send_key_status(TOUCH_KEY_CLOCK);
                }

                // 按键松开，发送对应信息
                // uart1_sendbyte(0x30);
                // uart1_sendbyte(~0x30);
                return;
            }
            else if (TK_CH11_VALIB == KeyOnOutput)
            {
                // 如果按键--电源被触摸

                // 如果不是同一个按键按下或是没有值，说明是短按
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK4); // 发送带有状态的键值

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // 按键不松开，就一直发送数据
                    send_key_status(TOUCH_KEY_POWER);
                }

                // 按键松开，发送对应信息
                // uart1_sendbyte(0x40);
                // uart1_sendbyte(~0x40);
                return;
            }
            else if (TK_CH10_VALIB == KeyOnOutput)
            {
                // 如果按键--减被触摸

                // 如果不是同一个按键按下或是没有值，说明是短按
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK5); // 发送带有状态的键值

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // 按键不松开，就一直发送数据
                    send_key_status(TOUCH_KEY_SUB);
                }

                // 按键松开，发送对应信息
                // uart1_sendbyte(0x50);
                // uart1_sendbyte(~0x50);
                return;
            }
            else if (TK_CH9_VALIB == KeyOnOutput)
            {
                // 如果按键--加被触摸

                // 如果不是同一个按键按下或是没有值，说明是短按
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK6); // 发送带有状态的键值

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // 按键不松开，就一直发送数据
                    send_key_status(TOUCH_KEY_ADD);
                }

                // 按键松开，发送对应信息
                // uart1_sendbyte(0x60);
                // uart1_sendbyte(~0x60);
                return;
            }
        }
    }

#if 1 // 长按相关的功能
    if (1 == long_touch_flag && 0 != KeyOnOutput)
    {
        KeyOld = KeyOnOutput;

        // 如果之前是长按，现在还未松开
        // 判断当前触摸的键值，做对应的操作
        if (TK_CH2_VALIB == KeyOnOutput)
        {
            // 如果 按键--灯光 被触摸

            // 全局的长按标志位置一
            // is_touch_key_light_hold = 1;

            // 若还不松开手
            while (0 != KeyOnOutput)
            {
                __tk_scan();
                KeyOnOutput = __tk_key_flag;

#if 1 // 进行长按的灯光调节
                if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode)
                {
                    if (1 == light_on_off_status) // 只有灯亮时，才可以使用长按调节灯光
                    {
                        // 如果当前是单色模式（只控制白灯）
                        if (light_adjust_add_sub_flag)
                        {
                            // 如果当前是在增大亮度
                            if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                            {
                                tmr1_set_pwm_duty(++cur_pwm_duty);
                                cur_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                            }
                            else
                            {
                                // 如果已经到了最大亮度
                                tmr1_set_pwm_duty(cur_pwm_duty);
                            }
                        }
                        else
                        {
                            // 如果当前是在减小亮度
                            if (cur_pwm_duty > PWM_MIN_DUTY_LIGHT)
                            {
                                tmr1_set_pwm_duty(--cur_pwm_duty);
                                cur_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                            }
                            else
                            {
                                // 如果已经到了最小亮度
                                tmr1_set_pwm_duty(cur_pwm_duty);
                            }
                        }
                    }
                    else if (0 == light_on_off_status)
                    {
                        // 如果当前灯是关闭的，松开手时，把灯打开
                        is_touch_key_light_press = 1;
                        light_adjust_add_sub_flag = 1;
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

                            // if (0 == light_on_off_status)
                            // {
                            //     // 如果此时灯光是灭的，打开灯光，并更新状态
                            //     tmr1_set_pwm_duty(cur_pwm_duty);
                            //     white_light_open();

                            //     light_on_off_status = 1; // 更新灯光状态
                            // }

                            if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                            {
                                tmr1_set_pwm_duty(++cur_pwm_duty);
                                cur_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                            }
                            else
                            {
                                // 如果已经到了最大亮度
                            }
                        }
                        else
                        {
                            // 如果当前是在减小亮度
                            if (cur_pwm_duty > PWM_MIN_DUTY_LIGHT)
                            {
                                tmr1_set_pwm_duty(--cur_pwm_duty);
                                cur_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                            }
                            else
                            {
                                // 如果已经到了最小亮度
                                // tmr1_set_pwm_duty(cur_pwm_duty);
                                // white_light_close();
                                // light_on_off_status = 0; // 更新灯光状态
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status)
                    {
                        // 如果当前是黄灯
                        if (light_adjust_add_sub_flag)
                        {
                            // 如果当前是在增大亮度

                            // if (0 == light_on_off_status)
                            // {
                            //     // 如果此时灯光是灭的，打开灯光，并更新状态
                            //     tmr0_set_pwm_duty(cur_pwm_duty);
                            //     yellow_light_open();
                            //     light_on_off_status = 1; // 更新灯光状态
                            // }

                            if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                            {
                                tmr0_set_pwm_duty(++cur_pwm_duty);
                                cur_yellow_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                            }
                            else
                            {
                                // 如果已经到了最大亮度
                            }
                        }
                        else
                        {
                            // 如果当前是在减小亮度
                            if (cur_pwm_duty > PWM_MIN_DUTY_LIGHT)
                            {
                                tmr0_set_pwm_duty(--cur_pwm_duty);
                                cur_yellow_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                                // yellow_light_open();
                                // light_on_off_status = 1; // 更新灯光状态
                            }
                            else
                            {
                                // 如果已经到了最小亮度
                                // cur_pwm_duty = 0;
                                // tmr0_set_pwm_duty(cur_pwm_duty);
                                // yellow_light_close();
                                // light_on_off_status = 0; // 更新灯光状态
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
                    {
                        // 如果当前是黄白色
                        if (light_adjust_add_sub_flag)
                        {
                            // 如果当前是在增大亮度

                            // if (0 == light_on_off_status)
                            // {
                            //     // 如果此时灯光是灭的，打开灯光，并更新状态
                            //     tmr0_set_pwm_duty(cur_pwm_duty);
                            //     tmr1_set_pwm_duty(cur_pwm_duty);
                            //     yellow_light_open();
                            //     white_light_open();

                            //     light_on_off_status = 1; // 更新灯光状态
                            // }

                            if (cur_pwm_duty < PWM_MAX_DUTY_MUILTY_LIGHT)
                            {
                                tmr0_set_pwm_duty(++cur_pwm_duty);
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                cur_yellow_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                                // yellow_light_open();
                                // white_light_open();
                            }
                            else
                            {
                                // 如果已经到了最大亮度
                            }
                        }
                        else
                        {
                            // 如果当前是在减小亮度
                            if (cur_pwm_duty > PWM_MIN_DUTY_YELLOW_WHITE_LIGHT)
                            {
                                tmr0_set_pwm_duty(--cur_pwm_duty);
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                 cur_yellow_white_duty = cur_pwm_duty; // 更新独立保存的PWM占空比
                                // yellow_light_open();
                                // white_light_open();
                                // light_on_off_status = 1; // 更新灯光状态
                            }
                            else
                            {
                                // 如果已经到了最小亮度
                                // tmr0_set_pwm_duty(cur_pwm_duty);
                                // tmr1_set_pwm_duty(cur_pwm_duty);
                                // yellow_light_close();
                                // white_light_close();
                                // light_on_off_status = 0; // 更新灯光状态
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status)
                    {
                        // 如果当前灯是关闭的，松开手时，把灯打开
                        is_touch_key_light_press = 1;
                        light_adjust_add_sub_flag = 1;
                    }
                }
#endif // 进行长按的灯光调节

                // 在这里发送按键数据
                send_key_status(TOUCH_KEY_LIGHT);
                // delay_100us(15); // 每1.5ms调整0.1%的占空比（直接在这里调节灯光亮度才使用，在定时器里调节灯光亮度可以注释掉这一行）
                delay_100us(PWM_ADJUST_DUTY_TIME_STEP); // 每x.xms调整0.1%的占空比
            } // 如果还不松手

            // 如果长按后松手了，要更新当前是在增大亮度还是减少亮度的标志位
            if (light_adjust_add_sub_flag)
            {
                // 如果之前是在增大亮度，下次长按要变成减小亮度
                light_adjust_add_sub_flag = 0; // 更新标志位
            }
            else
            {
                // 如果之前是在减小亮度，下次长按要变成增大亮度
                light_adjust_add_sub_flag = 1; // 更新标志位
            }
        }
    }
    else if (1 == long_touch_flag && 0 == KeyOnOutput)
    {
        // 如果之前是长按，现在却松开了按键
        // switch (KeyOld)
        // {
        // case TK_CH25_VALIB:
        //     // send_status_keyval(KEY_PRESS_LOOSE, KEY_TK1);
        //     break;

        // case TK_CH3_VALIB:
        //     // send_status_keyval(KEY_PRESS_LOOSE, KEY_TK2);
        //     break;

        // case TK_CH2_VALIB:
        //     // send_status_keyval(KEY_PRESS_LOOSE, KEY_TK3);
        //     break;

        // case TK_CH0_VALIB:
        //     // send_status_keyval(KEY_PRESS_LOOSE, KEY_TK4);
        //     break;

        // case TK_CH9_VALIB:
        //     // send_status_keyval(KEY_PRESS_LOOSE, KEY_TK5);
        //     break;

        // case TK_CH10_VALIB:
        //     // send_status_keyval(KEY_PRESS_LOOSE, KEY_TK6);
        //     break;
        // }

        // is_touch_key_light_hold = 0; // 清除灯光按键长按的标志位，表示未长按

        long_touch_flag = 0; // 清除标志位
        cnt = 0;
        KeyOld = 0;
        __tk_key_flag = 0;
    }
#endif // 长按相关的功能
}
#endif // end of #ifdef CIRCUIT_BOARD

/*************************** (C) COPYRIGHT 2022 TAIXIN-IC ***** END OF FILE *****/

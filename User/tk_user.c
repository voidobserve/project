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
xdata u16 led_data[1];                           // LED��ʾbuff
#define LED_DIS_FREQ (1000000 / 16 / 8 / 60 - 1) // һ��CON����ʾʱ��60Hz    60-1=1M/(LED_COMCON*16*LED_TIME_CON)

unsigned int xdata display_data = 0;
#endif // #ifdef DEVELOPMENT_BOARD

/**
 * @brief  ���Խӿ�IO��ʼ������.
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
// �ڿ������ϣ�
// P2_7���ӵ��ĸ���ɫLED�ĸ����Ĺ�����
// P0_3���ӵ�LED1����
// P3_0���ӵ�LED2����
// P1_0���ӵ�LED3����
// P0_7���ӵ�LED4����
void led_init(void)
{
    // ����COM0   P27Ϊ���ģʽ
    // P2_MD1��P2�Ĺ���ģʽ�Ĵ���1
    P2_MD1 &= ~GPIO_P27_MODE_SEL(0x3); // ����P2_MD1�Ĵ�����6-7λ
    P2_MD1 |= GPIO_P27_MODE_SEL(0x1);  // P2_MD1��6~7λ����Ϊ0x01�������ὫP2�ĵ�7��IO����Ϊ���ģʽ

    // ����IO����������
    // P2�ĵ�7��IO����������Ϊ8mA
    P2_DRV7 = GPIO_P27_DRV_SEL(0x1);

    // ����SEG0-SEG4 P03\P30\P10\P07-���ģʽ
    // P0_3��P3_0��P1_0��P0_1����Ϊ���ģʽ
    // �����������㣬����λ�Ĳ���
    P0_MD0 &= ~GPIO_P03_MODE_SEL(0x3);
    P3_MD0 &= ~GPIO_P30_MODE_SEL(0x3);
    P1_MD0 &= ~GPIO_P10_MODE_SEL(0x3);
    P0_MD1 &= ~GPIO_P07_MODE_SEL(0x3);

    P0_MD0 |= GPIO_P03_MODE_SEL(0x1);
    P3_MD0 |= GPIO_P30_MODE_SEL(0x1);
    P1_MD0 |= GPIO_P10_MODE_SEL(0x1);
    P0_MD1 |= GPIO_P07_MODE_SEL(0x1);

    // SEG0-SEG4 P03\P30\P10\P07
    // ����P0_3��P3_0��P1_0��P0_1��������ܣ�����LED��Ӧ�������ӵ���Ƭ������LEDģ�飬���ģ���Ǹ���������
    FOUT_S03 = GPIO_FOUT_LED_SEG0;
    FOUT_S30 = GPIO_FOUT_LED_SEG1;
    FOUT_S10 = GPIO_FOUT_LED_SEG2;
    FOUT_S07 = GPIO_FOUT_LED_SEG3;

    // ����P0_3��P3_0��P1_0��P0_1�����������8mA
    P0_DRV3 = GPIO_P03_DRV_SEL(0x1);
    P3_DRV0 = GPIO_P30_DRV_SEL(0x1);
    P1_DRV0 = GPIO_P10_DRV_SEL(0x1);
    P0_DRV7 = GPIO_P07_DRV_SEL(0x1);

    // COM0 P27
    // ����P2_7��������ܣ����ӵ�LED�Ĺ�����0
    FOUT_S27 = GPIO_FOUT_LED_COM0;

    CLK_CON2 |= CLK_LED_EN(0x1); // ��LEDģ���ʱ�ӣ���Ƭ������LEDģ���ʱ�ӣ�

    // 0x55��ʾ������ʺ����� wdt_psr����WDT_CON�У�psr��ʾ���Ź���ʱʱ�䣩
    WDT_KEY = 0x55;                // ����IO_MAP�Ĵ�����ҪдKEY
    IO_MAP |= MAP_LED_DMA_EN(0x1); // LED DMAʹ��
    WDT_KEY = 0xBB;                // д��һ���������ֵ���ָ��üĴ�����д����

    // ��LEDģ���ʱ��
    CLK_CON2 |= 0x10;

    LED_SEGCONL = LED_SEG0_EN(0x1) | // SEG0 ʹ��
                  LED_SEG1_EN(0x1) | // SEG0 ʹ��
                  LED_SEG2_EN(0x1) | // SEG0 ʹ��
                  LED_SEG3_EN(0x1);  // SEG0 ʹ��
    LED_COMCON = LED_COM0_EN(0x1);   // COM1 ʹ��

    LED_DMAADRH = LED_DMA_ADDR_H((((u16)led_data) >> 8) & 0xFF); // LED���ݴ����׵�ַ�ĸ�8λ
    LED_DMAADRL = LED_DMA_ADDR_L((((u16)led_data) >> 0) & 0xFF); // LED���ݴ����׵�ַ�ĵ�8λ
    LED_TIMECON = LED_SCAN_TIME(LED_DIS_FREQ);                   // ɨ�赽ĳ��com��segʱ������ʱ��,����Ϊ32΢��
    LED_CON = LED_COM_SEG_SEL(0x0) |                             // ɨ�跽ʽѡ��comɨ��
              LED_EN(0x1);                                       // LEDʹ��,ʹ��֮���DMAADDRH��DMAADDRL�ĵ�ַ��ʼ������ɨ�����,�û�������д����Ӧ�ĵ�ַ����

    led_data[0] = 0x00000f00; // һ��ʼ���ĸ�LEDȫ������
    // led_data[0] = 0x00000000; // һ��ʼ���ĸ�LEDȫ��Ϩ��
}

void led_display(void)
{
    led_data[0] = display_data;
}
#endif // #ifdef DEVELOPMENT_BOARD

/**
 * @brief  �û������ʼ�������ӿ�.
 * @param  None
 * @retval None
 */
void user_init(void)
{
    // led_init(); // ��ʼ��������LED��ص�����

#ifdef DEVELOPMENT_BOARD // demo����ʹ�ô���0����Ϊ����
    uart0_config();      // �����õ��Ĵ���
    p24_output_config(); // �����õģ��۲����ŵĵ�ƽ�仯
#endif                   // demo����ʹ�ô���0����Ϊ����

    uart1_config(); // ����ͨ�ŵ�����
    tmr2_config();  // ��ʱ��2�����ã�������ɺ�Ĭ���ǹرյ�
    knob_config();  // ��ť������
    // tmr2_enable();

#if USE_MY_DEBUG
    // p25_output_config(); //
#endif // USE_MY_DEBUG
}

#ifdef DEVELOPMENT_BOARD // �������϶�Ӧ������������ӿ�
/**
 * @brief  �û�����ѭ��ִ�к����ӿ�.����������ɨ�躯���ӿڣ�
 * @param  None
 * @retval None
 */
void user_handle(void)
{
    volatile unsigned long int KeyOnOutput = __tk_key_flag; // __tk_key_flag���ΰ�����־
    static volatile unsigned long int KeyOld = 0;           // ���ڴ�ų���ʱ����Ӧ�İ���

    // �Ƿ񳤰��ı�־��0--��1--��
    // ��Ҫ���泤����״̬����ʾ��ʱ��һֱ�ڷ��ͼ�ֵ
    static unsigned char long_touch_flag = 0; // ��һ��ɨ�赽�İ����Ƿ�Ϊ�����ı�־
    static volatile u32 cnt = 0;              // ��������ֵ

    u32 i = 0; // ѭ������ֵ

    if (KeyOnOutput && 0 == long_touch_flag && cnt < TK_LONG_KEY_TIME) // ����а������£�����֮ǰɨ�赽�Ĳ��ǳ�����
    {
#if 1
        // ͨ��ѭ���ж��Ƿ�Ϊ����
        for (i = 0; i < 500000; i++) // ʵ�����ò�����ô������֣������趨�ĳ���ʱ��ͻ��˳�
        {
            // ����ɨ�躯����ʹ���˿�����Ľӿڣ���Դ�⣩
            // ������������__tk_key_flag��ֵ
            delay_ms(10);
            __tk_scan();

            if (KeyOnOutput == __tk_key_flag)
            {
                // ���������ֵһ�£�˵������һֱ�����£�δ�ɿ�
                cnt++;

                if (TK_CH0_VALIB == KeyOnOutput)
                {
                    printf("��⵽����1���£����һ������Ƿ񳤰����ж���\n");
                }
                else
                {
                    // ������������������ü�ⳤ��
                    break;
                }
            }
            else
            {
                // ���������ֵ��һ�»����ɿ��˰���
                break;
            }
            if (cnt > TK_LONG_KEY_TIME) // ��������������������õ�ʱ��
            {
                long_touch_flag = 1; // ���Ϊ����
                break;
            }
        }
#endif

        if (cnt > TK_LONG_KEY_TIME)
        {
            long_touch_flag = 1; // ���Ϊ����
        }
        else
        {
            // ����Ƕ̰�
            cnt = 0; // ��������ļ���ֵ

            // �жϵ�ǰ�����ļ�ֵ������Ӧ�Ĳ���
            if (TK_CH0_VALIB == KeyOnOutput)
            {

                // �ȴ���ָ�ɿ���������Ȼ���������˫��״̬�ļ�ֵ����ٷ���һ�ε���״̬�ļ�ֵ
                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // �������ɿ�����һֱ��������
                    //  send_key_status(TOUCH_KEY_BULETOOTH);
                    printf("����1����\n");
                }

                // �����ɿ�ʱ������ʾ�������µ���Ϣ
                printf("����1���²��ɿ�\n");
                is_touch_key_light_press = 1;

                return;
            }
            else if (TK_CH4_VALIB == KeyOnOutput)
            {
                // ����������TK2������
                // display_data ^= 0x0200; // ��Ӧ��LED״̬ȡ������������led_display()ˢ��
                for (i = 0; i < 300; i++)
                {
                    delay_ms(1);
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;

                    if (TK_CH4_VALIB == KeyOnOutput)
                    {
                        // �����⵽��ͬһ���������£�˵����˫��
                        // �ȴ���ָ�ɿ���������Ȼ���������˫��״̬�ļ�ֵ����ٷ���һ�ε���״̬�ļ�ֵ
                        while (0 != KeyOnOutput)
                        {
                            __tk_scan();
                            KeyOnOutput = __tk_key_flag;
                        }

                        // send_status_keyval(KEY_PRESS_DOUBLECLICK, KEY_TK2);
                        return;
                    }
                }

                // �������ͬһ���������»���û��ֵ��˵���Ƕ̰�
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK2); // ���ʹ���״̬�ļ�ֵ
            }
            else if (TK_CH5_VALIB == KeyOnOutput)
            {
                // ����������TK3������
                // display_data ^= 0x0400; // ��Ӧ��LED״̬ȡ������������led_display()ˢ��
                for (i = 0; i < 300; i++)
                {
                    delay_ms(1);
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;

                    if (TK_CH5_VALIB == KeyOnOutput)
                    {
                        // �����⵽��ͬһ���������£�˵����˫��
                        // �ȴ���ָ�ɿ���������Ȼ���������˫��״̬�ļ�ֵ����ٷ���һ�ε���״̬�ļ�ֵ
                        while (0 != KeyOnOutput)
                        {
                            __tk_scan();
                            KeyOnOutput = __tk_key_flag;
                        }

                        // send_status_keyval(KEY_PRESS_DOUBLECLICK, KEY_TK3);
                        return;
                    }
                }

                // �������ͬһ���������»���û��ֵ��˵���Ƕ̰�
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK3); // ���ʹ���״̬�ļ�ֵ
            }
            else if (TK_CH6_VALIB == KeyOnOutput)
            {
                // ����������TK4������
                // display_data ^= 0x0800; // ��Ӧ��LED״̬ȡ������������led_display()ˢ��
                for (i = 0; i < 300; i++)
                {
                    delay_ms(1);
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;

                    if (TK_CH6_VALIB == KeyOnOutput)
                    {
                        // �����⵽��ͬһ���������£�˵����˫��
                        // �ȴ���ָ�ɿ���������Ȼ���������˫��״̬�ļ�ֵ����ٷ���һ�ε���״̬�ļ�ֵ
                        while (0 != KeyOnOutput)
                        {
                            __tk_scan();
                            KeyOnOutput = __tk_key_flag;
                        }

                        // send_status_keyval(KEY_PRESS_DOUBLECLICK, KEY_TK4);
                        return;
                    }
                }

                // �������ͬһ���������»���û��ֵ��˵���Ƕ̰�
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK4); // ���ʹ���״̬�ļ�ֵ
            }
        }
    }

#if 1
    if (1 == long_touch_flag && 0 != KeyOnOutput)
    {
        KeyOld = KeyOnOutput;

        // ���֮ǰ�ǳ��������ڻ�δ�ɿ�
        // �жϵ�ǰ�����ļ�ֵ������Ӧ�Ĳ���
        if (TK_CH0_VALIB == KeyOnOutput)
        {
            // ����������TK1������
            if (cnt >= TK_LONG_KEY_TIME && cnt < 90)
            {
                printf("����1����\n");
            }

#if USE_MY_DEBUG
            P24 = ~P24;
#endif // USE_MY_DEBUG

            // �������ɿ���
            while (0 != KeyOnOutput)
            {
                __tk_scan();
                KeyOnOutput = __tk_key_flag;

                // ���еƹ����
                if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode)
                {
                    // �����ǰ�ǵ�ɫģʽ��ֻ���ư׵ƣ�
                    if (light_adjust_add_sub_flag)
                    {
                        // �����ǰ������������
                        if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                        {
                            tmr1_set_pwm_duty(++cur_pwm_duty);
                            white_light_open();
                        }
                        else
                        {
                            // ����Ѿ������������
                            // cur_pwm_duty = 700;
                            tmr1_set_pwm_duty(cur_pwm_duty);
                            white_light_open();
#if USE_MY_DEBUG
                            P24 = ~P24;
#endif // USE_MY_DEBUG
                        }

                        light_on_off_status = 1; // ���µƹ�״̬
                    }
                    else
                    {
                        // �����ǰ���ڼ�С����
                        if (cur_pwm_duty > 0)
                        {
                            tmr1_set_pwm_duty(--cur_pwm_duty);
                            white_light_open();

                            light_on_off_status = 1; // ���µƹ�״̬
                        }
                        else
                        {
                            // ����Ѿ�������С����
                            // cur_pwm_duty = 0;
                            tmr1_set_pwm_duty(cur_pwm_duty);
                            white_light_close();

                            light_on_off_status = 0; // ���µƹ�״̬
#if USE_MY_DEBUG
                            P24 = ~P24;
#endif // USE_MY_DEBUG
                        }
                    }
                }
                else if (LIGHT_CTL_MODE_MUILTY == light_ctl_mode)
                {
                    // �����ǰ����ɫ�л�ģʽ
                    // �жϵ�ǰ�ǲ��ǻư�ɫ������ǣ���Ҫע������ռ�ձ�Ϊ60%��������ǣ�����ռ�ձ�Ϊ70%

                    if (LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status)
                    {
                        // �����ǰ�ǰ׵�
                        if (light_adjust_add_sub_flag)
                        {
                            // �����ǰ������������
                            if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                            {
                                tmr1_set_pwm_duty(++cur_pwm_duty);
                                white_light_open();
                            }
                            else
                            {
                                // ����Ѿ������������
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                white_light_open();
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
                            }

                            light_on_off_status = 1; // ���µƹ�״̬
                        }
                        else
                        {
                            // �����ǰ���ڼ�С����
                            if (cur_pwm_duty > 0)
                            {
                                tmr1_set_pwm_duty(--cur_pwm_duty);
                                white_light_open();
                                light_on_off_status = 1; // ���µƹ�״̬
                            }
                            else
                            {
                                // ����Ѿ�������С����
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                white_light_close();
                                light_on_off_status = 0; // ���µƹ�״̬
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status)
                    {
                        // �����ǰ�ǻƵ�
                        if (light_adjust_add_sub_flag)
                        {
                            // �����ǰ������������
                            if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                            {
                                tmr0_set_pwm_duty(++cur_pwm_duty);
                                yellow_light_open();
                            }
                            else
                            {
                                // ����Ѿ������������
                                tmr0_set_pwm_duty(cur_pwm_duty);
                                yellow_light_open();
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
                            }
                            light_on_off_status = 1; // ���µƹ�״̬
                        }
                        else
                        {
                            // �����ǰ���ڼ�С����
                            if (cur_pwm_duty > 0)
                            {
                                tmr0_set_pwm_duty(--cur_pwm_duty);
                                yellow_light_open();
                                light_on_off_status = 1; // ���µƹ�״̬
                            }
                            else
                            {
                                // ����Ѿ�������С����
                                // cur_pwm_duty = 0;
                                tmr0_set_pwm_duty(cur_pwm_duty);
                                yellow_light_close();
                                light_on_off_status = 0; // ���µƹ�״̬
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
                    {
                        // �����ǰ�ǻư�ɫ
                        if (light_adjust_add_sub_flag)
                        {
                            // �����ǰ������������
                            if (cur_pwm_duty < PWM_MAX_DUTY_MUILTY_LIGHT)
                            {
                                tmr0_set_pwm_duty(++cur_pwm_duty);
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                yellow_light_open();
                                white_light_open();
                            }
                            else
                            {
                                // ����Ѿ������������
                                tmr0_set_pwm_duty(cur_pwm_duty);
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                yellow_light_open();
                                white_light_open();
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
                            }
                            light_on_off_status = 1; // ���µƹ�״̬
                        }
                        else
                        {
                            // �����ǰ���ڼ�С����
                            if (cur_pwm_duty > 0)
                            {
                                tmr0_set_pwm_duty(--cur_pwm_duty);
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                yellow_light_open();
                                white_light_open();
                                light_on_off_status = 1; // ���µƹ�״̬
                            }
                            else
                            {
                                // ����Ѿ�������С����
                                tmr0_set_pwm_duty(cur_pwm_duty);
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                yellow_light_close();
                                white_light_close();
                                light_on_off_status = 0; // ���µƹ�״̬
#if USE_MY_DEBUG
                                P24 = ~P24;
#endif // USE_MY_DEBUG
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status)
                    {
                        // �����ǰ���ǹرյģ��ɿ���ʱ���ѵƴ�
                        is_touch_key_light_press = 1;
                        light_adjust_add_sub_flag = 1;
                        // return;
                    }
                }

                // �����﷢�Ͱ�������
                delay_100us(15); // ÿ1.5ms����0.1%��ռ�ձ�
            }

            // ��������������ˣ�Ҫ���µ�ǰ�����������Ȼ��Ǽ������ȵı�־λ
            if (light_adjust_add_sub_flag)
            {
                // ���֮ǰ�����������ȣ��´γ���Ҫ��ɼ�С����
                light_adjust_add_sub_flag = 0; // ���±�־λ
            }
            else
            {
                // ���֮ǰ���ڼ�С���ȣ��´γ���Ҫ�����������
                light_adjust_add_sub_flag = 1; // ���±�־λ
            }
        }
    }
    else if (1 == long_touch_flag && 0 == KeyOnOutput)
    {
        // ���֮ǰ�ǳ���������ȴ�ɿ��˰���
        switch (KeyOld)
        {
        case TK_CH0_VALIB:
            printf("����1�������ɿ��˰���\n");
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

        long_touch_flag = 0; // �����־λ
        cnt = 0;
        KeyOld = 0;
    }
#endif

    // led_display(); // LED״̬������ʾ
}
#endif // end ifdef DEVELOPMENT_BOARD

#ifdef CIRCUIT_BOARD // Ŀ���·���Ӧ�Ĵ������������ӿ�
/**
 * @brief  �û�����ѭ��ִ�к����ӿ�.����������ɨ�躯���ӿڣ�
 * @param  None
 * @retval None
 */
void user_handle(void)
{
    volatile unsigned long int KeyOnOutput = __tk_key_flag; // __tk_key_flag���ΰ�����־
    static volatile unsigned long int KeyOld = 0;           // ���ڴ�ų���ʱ����Ӧ�İ���

    // �Ƿ񳤰��ı�־��0--��1--��
    // ��Ҫ���泤����״̬����ʾ��ʱ��һֱ�ڷ��ͼ�ֵ
    static unsigned char long_touch_flag = 0; // ��һ��ɨ�赽�İ����Ƿ�Ϊ�����ı�־
    static volatile u32 cnt = 0;              // ��������ֵ

    u32 i = 0; // ѭ������ֵ

    if (KeyOnOutput && 0 == long_touch_flag && cnt < TK_LONG_KEY_TIME) // ����а������£�����֮ǰɨ�赽�Ĳ��ǳ�����
    {
        // ͨ��ѭ���ж��Ƿ�Ϊ����
        for (i = 0; i < 500000; i++) // ʵ�����ò�����ô������֣������趨�ĳ���ʱ��ͻ��˳�
        {
            // ����ɨ�躯����ʹ���˿�����Ľӿڣ���Դ�⣩
            // ������������__tk_key_flag��ֵ
            delay_ms(10);
            __tk_scan();

            if (KeyOnOutput == __tk_key_flag)
            {
                // ���������ֵһ�£�˵������һֱ�����£�δ�ɿ�
                cnt++;

                // ����ǵƹⰴ�����£��������Ͱ�����״̬
                if (TK_CH2_VALIB == KeyOnOutput)
                {
                    send_key_status(TOUCH_KEY_LIGHT);
                }
                else
                {
                    // ������������������ü�ⳤ��
                    break;
                }
            }
            else
            {
                // ���������ֵ��һ�»����ɿ��˰���
                // cnt = 0;
                break;
            }

            if (cnt > TK_LONG_KEY_TIME) // ��������������������õ�ʱ��
            {
                long_touch_flag = 1; // ���Ϊ����
                break;
            }
        }

        if (cnt > TK_LONG_KEY_TIME)
        {
            long_touch_flag = 1; // ���Ϊ����
        }
        else
        {
            // ����Ƕ̰�
            cnt = 0; // ��������ļ���ֵ

            // �жϵ�ǰ�����ļ�ֵ������Ӧ�Ĳ���
            if (TK_CH3_VALIB == KeyOnOutput)
            {
                // ��������˰���--����

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // �������ɿ�����һֱ��������
                    send_key_status(TOUCH_KEY_BULETOOTH);
                }

                // �����ɿ������Ͷ�Ӧ��Ϣ
                // uart1_sendbyte(0x10);
                // uart1_sendbyte(~0x10);
                return;
            }
            else if (TK_CH2_VALIB == KeyOnOutput)
            {
                // �������--�Ʊ�����

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // �������ɿ�����һֱ��������
                    send_key_status(TOUCH_KEY_LIGHT);
                }

                // �����ɿ������Ͷ�Ӧ��Ϣ
                // uart1_sendbyte(0x20);
                // uart1_sendbyte(~0x20);

                is_touch_key_light_press = 1; // ��־λ��һ
                return;
            }
            else if (TK_CH0_VALIB == KeyOnOutput)
            {
                // �������--ʱ�ӱ�����

                // �������ͬһ���������»���û��ֵ��˵���Ƕ̰�
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK3); // ���ʹ���״̬�ļ�ֵ

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // �������ɿ�����һֱ��������
                    send_key_status(TOUCH_KEY_CLOCK);
                }

                // �����ɿ������Ͷ�Ӧ��Ϣ
                // uart1_sendbyte(0x30);
                // uart1_sendbyte(~0x30);
                return;
            }
            else if (TK_CH11_VALIB == KeyOnOutput)
            {
                // �������--��Դ������

                // �������ͬһ���������»���û��ֵ��˵���Ƕ̰�
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK4); // ���ʹ���״̬�ļ�ֵ

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // �������ɿ�����һֱ��������
                    send_key_status(TOUCH_KEY_POWER);
                }

                // �����ɿ������Ͷ�Ӧ��Ϣ
                // uart1_sendbyte(0x40);
                // uart1_sendbyte(~0x40);
                return;
            }
            else if (TK_CH10_VALIB == KeyOnOutput)
            {
                // �������--��������

                // �������ͬһ���������»���û��ֵ��˵���Ƕ̰�
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK5); // ���ʹ���״̬�ļ�ֵ

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // �������ɿ�����һֱ��������
                    send_key_status(TOUCH_KEY_SUB);
                }

                // �����ɿ������Ͷ�Ӧ��Ϣ
                // uart1_sendbyte(0x50);
                // uart1_sendbyte(~0x50);
                return;
            }
            else if (TK_CH9_VALIB == KeyOnOutput)
            {
                // �������--�ӱ�����

                // �������ͬһ���������»���û��ֵ��˵���Ƕ̰�
                // send_status_keyval(KEY_PRESS_SHORT, KEY_TK6); // ���ʹ���״̬�ļ�ֵ

                while (0 != KeyOnOutput)
                {
                    __tk_scan();
                    KeyOnOutput = __tk_key_flag;
                    // �������ɿ�����һֱ��������
                    send_key_status(TOUCH_KEY_ADD);
                }

                // �����ɿ������Ͷ�Ӧ��Ϣ
                // uart1_sendbyte(0x60);
                // uart1_sendbyte(~0x60);
                return;
            }
        }
    }

#if 1 // ������صĹ���
    if (1 == long_touch_flag && 0 != KeyOnOutput)
    {
        KeyOld = KeyOnOutput;

        // ���֮ǰ�ǳ��������ڻ�δ�ɿ�
        // �жϵ�ǰ�����ļ�ֵ������Ӧ�Ĳ���
        if (TK_CH2_VALIB == KeyOnOutput)
        {
            // ��� ����--�ƹ� ������

            // ȫ�ֵĳ�����־λ��һ
            // is_touch_key_light_hold = 1;

            // �������ɿ���
            while (0 != KeyOnOutput)
            {
                __tk_scan();
                KeyOnOutput = __tk_key_flag;

#if 1 // ���г����ĵƹ����
                if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode)
                {
                    if (1 == light_on_off_status) // ֻ�е���ʱ���ſ���ʹ�ó������ڵƹ�
                    {
                        // �����ǰ�ǵ�ɫģʽ��ֻ���ư׵ƣ�
                        if (light_adjust_add_sub_flag)
                        {
                            // �����ǰ������������
                            if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                            {
                                tmr1_set_pwm_duty(++cur_pwm_duty);
                                cur_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                            }
                            else
                            {
                                // ����Ѿ������������
                                tmr1_set_pwm_duty(cur_pwm_duty);
                            }
                        }
                        else
                        {
                            // �����ǰ���ڼ�С����
                            if (cur_pwm_duty > PWM_MIN_DUTY_LIGHT)
                            {
                                tmr1_set_pwm_duty(--cur_pwm_duty);
                                cur_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                            }
                            else
                            {
                                // ����Ѿ�������С����
                                tmr1_set_pwm_duty(cur_pwm_duty);
                            }
                        }
                    }
                    else if (0 == light_on_off_status)
                    {
                        // �����ǰ���ǹرյģ��ɿ���ʱ���ѵƴ�
                        is_touch_key_light_press = 1;
                        light_adjust_add_sub_flag = 1;
                    }
                }
                else if (LIGHT_CTL_MODE_MUILTY == light_ctl_mode)
                {
                    // �����ǰ����ɫ�л�ģʽ
                    // �жϵ�ǰ�ǲ��ǻư�ɫ������ǣ���Ҫע������ռ�ձ�Ϊ60%��������ǣ�����ռ�ձ�Ϊ70%
                    if (LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status)
                    {
                        // �����ǰ�ǰ׵�
                        if (light_adjust_add_sub_flag)
                        {
                            // �����ǰ������������

                            // if (0 == light_on_off_status)
                            // {
                            //     // �����ʱ�ƹ�����ģ��򿪵ƹ⣬������״̬
                            //     tmr1_set_pwm_duty(cur_pwm_duty);
                            //     white_light_open();

                            //     light_on_off_status = 1; // ���µƹ�״̬
                            // }

                            if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                            {
                                tmr1_set_pwm_duty(++cur_pwm_duty);
                                cur_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                            }
                            else
                            {
                                // ����Ѿ������������
                            }
                        }
                        else
                        {
                            // �����ǰ���ڼ�С����
                            if (cur_pwm_duty > PWM_MIN_DUTY_LIGHT)
                            {
                                tmr1_set_pwm_duty(--cur_pwm_duty);
                                cur_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                            }
                            else
                            {
                                // ����Ѿ�������С����
                                // tmr1_set_pwm_duty(cur_pwm_duty);
                                // white_light_close();
                                // light_on_off_status = 0; // ���µƹ�״̬
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status)
                    {
                        // �����ǰ�ǻƵ�
                        if (light_adjust_add_sub_flag)
                        {
                            // �����ǰ������������

                            // if (0 == light_on_off_status)
                            // {
                            //     // �����ʱ�ƹ�����ģ��򿪵ƹ⣬������״̬
                            //     tmr0_set_pwm_duty(cur_pwm_duty);
                            //     yellow_light_open();
                            //     light_on_off_status = 1; // ���µƹ�״̬
                            // }

                            if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                            {
                                tmr0_set_pwm_duty(++cur_pwm_duty);
                                cur_yellow_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                            }
                            else
                            {
                                // ����Ѿ������������
                            }
                        }
                        else
                        {
                            // �����ǰ���ڼ�С����
                            if (cur_pwm_duty > PWM_MIN_DUTY_LIGHT)
                            {
                                tmr0_set_pwm_duty(--cur_pwm_duty);
                                cur_yellow_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                                // yellow_light_open();
                                // light_on_off_status = 1; // ���µƹ�״̬
                            }
                            else
                            {
                                // ����Ѿ�������С����
                                // cur_pwm_duty = 0;
                                // tmr0_set_pwm_duty(cur_pwm_duty);
                                // yellow_light_close();
                                // light_on_off_status = 0; // ���µƹ�״̬
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
                    {
                        // �����ǰ�ǻư�ɫ
                        if (light_adjust_add_sub_flag)
                        {
                            // �����ǰ������������

                            // if (0 == light_on_off_status)
                            // {
                            //     // �����ʱ�ƹ�����ģ��򿪵ƹ⣬������״̬
                            //     tmr0_set_pwm_duty(cur_pwm_duty);
                            //     tmr1_set_pwm_duty(cur_pwm_duty);
                            //     yellow_light_open();
                            //     white_light_open();

                            //     light_on_off_status = 1; // ���µƹ�״̬
                            // }

                            if (cur_pwm_duty < PWM_MAX_DUTY_MUILTY_LIGHT)
                            {
                                tmr0_set_pwm_duty(++cur_pwm_duty);
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                cur_yellow_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                                // yellow_light_open();
                                // white_light_open();
                            }
                            else
                            {
                                // ����Ѿ������������
                            }
                        }
                        else
                        {
                            // �����ǰ���ڼ�С����
                            if (cur_pwm_duty > PWM_MIN_DUTY_YELLOW_WHITE_LIGHT)
                            {
                                tmr0_set_pwm_duty(--cur_pwm_duty);
                                tmr1_set_pwm_duty(cur_pwm_duty);
                                 cur_yellow_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                                // yellow_light_open();
                                // white_light_open();
                                // light_on_off_status = 1; // ���µƹ�״̬
                            }
                            else
                            {
                                // ����Ѿ�������С����
                                // tmr0_set_pwm_duty(cur_pwm_duty);
                                // tmr1_set_pwm_duty(cur_pwm_duty);
                                // yellow_light_close();
                                // white_light_close();
                                // light_on_off_status = 0; // ���µƹ�״̬
                            }
                        }
                    }
                    else if (LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status)
                    {
                        // �����ǰ���ǹرյģ��ɿ���ʱ���ѵƴ�
                        is_touch_key_light_press = 1;
                        light_adjust_add_sub_flag = 1;
                    }
                }
#endif // ���г����ĵƹ����

                // �����﷢�Ͱ�������
                send_key_status(TOUCH_KEY_LIGHT);
                // delay_100us(15); // ÿ1.5ms����0.1%��ռ�ձȣ�ֱ����������ڵƹ����Ȳ�ʹ�ã��ڶ�ʱ������ڵƹ����ȿ���ע�͵���һ�У�
                delay_100us(PWM_ADJUST_DUTY_TIME_STEP); // ÿx.xms����0.1%��ռ�ձ�
            } // �����������

            // ��������������ˣ�Ҫ���µ�ǰ�����������Ȼ��Ǽ������ȵı�־λ
            if (light_adjust_add_sub_flag)
            {
                // ���֮ǰ�����������ȣ��´γ���Ҫ��ɼ�С����
                light_adjust_add_sub_flag = 0; // ���±�־λ
            }
            else
            {
                // ���֮ǰ���ڼ�С���ȣ��´γ���Ҫ�����������
                light_adjust_add_sub_flag = 1; // ���±�־λ
            }
        }
    }
    else if (1 == long_touch_flag && 0 == KeyOnOutput)
    {
        // ���֮ǰ�ǳ���������ȴ�ɿ��˰���
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

        // is_touch_key_light_hold = 0; // ����ƹⰴ�������ı�־λ����ʾδ����

        long_touch_flag = 0; // �����־λ
        cnt = 0;
        KeyOld = 0;
        __tk_key_flag = 0;
    }
#endif // ������صĹ���
}
#endif // end of #ifdef CIRCUIT_BOARD

/*************************** (C) COPYRIGHT 2022 TAIXIN-IC ***** END OF FILE *****/

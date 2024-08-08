// ��ʱ��TMR2������Դ�ļ�
#include "tmr2.h"

#define TMR2_CNT_TIME (SYSCLK / 32 / 10000 - 1) // �������� 10KHz��100us����һ���ж�

// volatile bit tmr2_flag = 0; // tmr2�жϷ������л���λ�ı�־λ
volatile u32 tmr2_cnt = 0; // ��ʱ��TMR2�ļ���ֵ��ÿ�����жϷ������л��һ��

/**
 * @brief ���ö�ʱ��TMR2
 */
void tmr2_config(void)
{
    TMR_ALLCON = TMR2_CNT_CLR(0x1);                           // �������ֵ
    TMR2_PRH = TMR_PERIOD_VAL_H((TMR2_CNT_TIME >> 8) & 0xFF); // ����ֵ
    TMR2_PRL = TMR_PERIOD_VAL_L((TMR2_CNT_TIME >> 0) & 0xFF);
    TMR2_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                               // ���TMR2�ļ�����־λ����ʾδ��ɼ���. ʹ��TMR2�ļ����ж�
    TMR2_CONL = /* TMR_SOURCE_SEL(0x7) |*/ TMR_PRESCALE_SEL(0x5) | TMR_MODE_SEL(0x1); // ѡ��ϵͳʱ�ӣ�32��Ƶ������ģʽ
    TMR2_CONL |= TMR_SOURCE_SEL(0x05);                                                // ����TMR2��ʱ��Դ�������κ�ʱ��
    IE_EA = 1;                                                                        // �����ж�
}

/**
 * @brief ������ʱ��TMR2����ʼ��ʱ
 */
void tmr2_enable(void)
{
    // ���¸�TMR2����ʱ��
    TMR2_CONL &= ~(TMR_SOURCE_SEL(0x07)); // �����ʱ����ʱ��Դ���üĴ���
    TMR2_CONL |= TMR_SOURCE_SEL(0x06);    // ���ö�ʱ����ʱ��Դ��ʹ��ϵͳʱ�ӣ�Լ21MHz��

    __EnableIRQ(TMR2_IRQn); // ʹ���ж�
    IE_EA = 1;              // �����ж�
}

/**
 * @brief �رն�ʱ��2����ռ���ֵ
 */
void tmr2_disable(void)
{
    // ������ʱ���ṩʱ�ӣ�����ֹͣ����
    TMR2_CONL &= ~(TMR_SOURCE_SEL(0x07)); // �����ʱ����ʱ��Դ���üĴ���
    TMR2_CONL |= TMR_SOURCE_SEL(0x05);    // ���ö�ʱ����ʱ��Դ�������κ�ʱ��

    // �����ʱ���ļ���ֵ
    TMR2_CNTL = 0;
    TMR2_CNTH = 0;

    __DisableIRQ(TMR2_IRQn); // �ر��жϣ���ʹ���жϣ�

    tmr2_cnt = 0; // ��������ļ���ֵ
}

// TMR2�жϷ�����
void TIMR2_IRQHandler(void) interrupt TMR2_IRQn
{
    static bit is_white_light_open = 0; // ��־λ�����ڿ��ư׵ƵĿ�������ֹ��ε��ÿ��������������˸

    // �����ж�����IP������ɾ��
    __IRQnIPnPush(TMR2_IRQn);
    // ---------------- �û��������� -------------------
    if (TMR2_CONH & TMR_PRD_PND(0x1))
    {
        TMR2_CONH |= TMR_PRD_PND(0x1); // ���pending
        // P24 = ~P24; // ���Զ�ʱ�������жϵ�����
        tmr2_cnt++; //

        // ���ݿ��Ƶƹ�ı�־λ��������Ӧ�Ĳ���
        switch (light_control_flag)
        {
#if 0                          // ����/�رջƵ�
            // case LIGHT_YELLOW_OPEN: // �����Ƶ�
            //     yellow_light_open();
            //     if (cur_pwm_duty < 700)
            //     {
            //         if (tmr2_cnt >= 10) // ÿ��1ms����PWMռ�ձ�����0.1%
            //         {
            //             tmr2_cnt = 0;
            //             tmr0_set_pwm_duty(++cur_pwm_duty);
            //         }
            //     }
            //     else
            //     {
            //         // ������ɺ������־λ
            //         light_control_flag = LIGHT_CONTROL_NONE;
            //     }
            //     break;
            // case LIGHT_YELLOW_CLOSE: // �رջƵ�
            //     if (cur_pwm_duty > 0)
            //     {
            //         if (tmr2_cnt >= 10) // ÿ��1ms����PWMռ�ձȼ���0.1%
            //         {
            //             tmr2_cnt = 0;
            //             tmr0_set_pwm_duty(--cur_pwm_duty);
            //         }
            //     }
            //     else
            //     {
            //         yellow_light_close(); //
            //         // ������ɺ������־λ
            //         light_control_flag = LIGHT_CONTROL_NONE;
            //     }
            //     break;
#endif                         // ����/�رջƵ�
        case LIGHT_WHITE_OPEN: // �򿪰׵�
            
            if (0 == is_white_light_open)
            {
                white_light_open();
                is_white_light_open = 1;
            }
                
            if (cur_pwm_duty < cur_white_duty)
            {
                // if (tmr2_cnt >= 10) // ÿ��1ms����PWMռ�ձ�����0.1%
                if (tmr2_cnt >= PWM_ADJUST_DUTY_TIME_STEP) 
                {
                    tmr2_cnt = 0;
                    tmr1_set_pwm_duty(++cur_pwm_duty);
                }
            }
            else
            {
                // ������ɺ������־λ
                light_control_flag = LIGHT_CONTROL_NONE;
            }
            break;
        case LIGHT_WHITE_CLOSE: // �رհ׵�
            if (cur_pwm_duty > 0)
            {
                // if (tmr2_cnt >= 10) // ÿ��1ms����PWMռ�ձȼ���0.1%
                if (tmr2_cnt >= PWM_ADJUST_DUTY_TIME_STEP) 
                {
                    tmr2_cnt = 0;
                    tmr1_set_pwm_duty(--cur_pwm_duty);
                }
            }
            else
            {
                white_light_close(); //
                
                // ������ɺ������־λ
                light_control_flag = LIGHT_CONTROL_NONE;
                is_white_light_open = 0;
            }
            break;

        default:          // Ĭ��ʲôҲ����
            tmr2_cnt = 0; // ��մ�Ŷ�ʱ������ֵ�ı���
            break;
        }

#if 0  // [����ʵ�ֹ��ܵĴ���]�����ж̰��������ĵƹ���ڶ����ڶ�ʱ����������ر𳤣����Ҳ��ô���
        if (is_touch_key_light_hold && tmr2_cnt >= 15)
        {
#if 1  // ���г����ĵƹ���ڣ�����ʱ��ÿ1.5ms����һ�Σ�
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
                        }
                    }
                }
                else if (LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status)
                {
                    // �����ǰ���ǹرյģ��������ɿ���ʱ���ѵƴ�
                    is_touch_key_light_press = 1;
                    light_adjust_add_sub_flag = 1;
                }
            }
#endif // ���г����ĵƹ����
        }
        else if (0 == is_touch_key_light_hold) // ������ǳ���
        {
            // ���ݿ��Ƶƹ�ı�־λ��������Ӧ�Ĳ���
            switch (light_control_flag)
            {
#if 0  // ����/�رջƵ�
       // case LIGHT_YELLOW_OPEN: // �����Ƶ�
       //     yellow_light_open();
       //     if (cur_pwm_duty < 700)
       //     {
       //         if (tmr2_cnt >= 10) // ÿ��1ms����PWMռ�ձ�����0.1%
       //         {
       //             tmr2_cnt = 0;
       //             tmr0_set_pwm_duty(++cur_pwm_duty);
       //         }
       //     }
       //     else
       //     {
       //         // ������ɺ������־λ
       //         light_control_flag = LIGHT_CONTROL_NONE;
       //     }
       //     break;
       // case LIGHT_YELLOW_CLOSE: // �رջƵ�
       //     if (cur_pwm_duty > 0)
       //     {
       //         if (tmr2_cnt >= 10) // ÿ��1ms����PWMռ�ձȼ���0.1%
       //         {
       //             tmr2_cnt = 0;
       //             tmr0_set_pwm_duty(--cur_pwm_duty);
       //         }
       //     }
       //     else
       //     {
       //         yellow_light_close(); //
       //         // ������ɺ������־λ
       //         light_control_flag = LIGHT_CONTROL_NONE;
       //     }
       //     break;
#endif // ����/�رջƵ�
            case LIGHT_WHITE_OPEN: // �򿪰׵�
                white_light_open();
                if (cur_pwm_duty < PWM_MAX_DUTY_SINGLE_LIGHT)
                {
                    if (tmr2_cnt >= 10) // ÿ��1ms����PWMռ�ձ�����0.1%
                    {
                        tmr2_cnt = 0;
                        tmr1_set_pwm_duty(++cur_pwm_duty);
                    }
                }
                else
                {
                    // ������ɺ������־λ
                    light_control_flag = LIGHT_CONTROL_NONE;
                }
                break;
            case LIGHT_WHITE_CLOSE: // �رհ׵�
                if (cur_pwm_duty > 0)
                {
                    if (tmr2_cnt >= 10) // ÿ��1ms����PWMռ�ձȼ���0.1%
                    {
                        tmr2_cnt = 0;
                        tmr1_set_pwm_duty(--cur_pwm_duty);
                    }
                }
                else
                {
                    white_light_close(); //
                    // ������ɺ������־λ
                    light_control_flag = LIGHT_CONTROL_NONE;
                }
                break;

            default:          // Ĭ��ʲôҲ����
                tmr2_cnt = 0; // ��մ�Ŷ�ʱ������ֵ�ı���
                break;
            }
        }
        // if (tmr2_cnt >= 100)
        // {
        //     tmr2_cnt = 0;

        //     if (light_on_off_status)
        //     {
        //         // ����ƹ��Ѿ�����
        //         if (cur_pwm_duty > 0)
        //         {
        //             tmr0_set_pwm_duty(cur_pwm_duty--);
        //         }
        //         else
        //         {
        //             tmr2_flag = 1; // ��ʾ�Ѿ��������
        //         }
        //     }
        //     else
        //     {
        //         // ����ƹ��Ѿ��ر�
        //         if (cur_pwm_duty < 700)
        //         {
        //             tmr0_set_pwm_duty(cur_pwm_duty++);
        //         }
        //         else
        //         {
        //             tmr2_flag = 1; // ��ʾ�Ѿ��������
        //         }
        //     }
        // }
#endif
    }

    // �˳��ж�����IP������ɾ��
    __IRQnIPnPop(TMR2_IRQn);
}

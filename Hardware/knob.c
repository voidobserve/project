// encoding GB2312
#include "knob.h"

static volatile u16 last_adc_val = 0;         // ��¼֮ǰ����adcֵ������б仯����Ҫ���������ƹ�����
static volatile bit last_knob_on_off_pin = 0; // ��¼��ť��/�����ŵĵ�ƽ״̬������б仯��˵��Ҫִ�п���/�صƲ���
// ��ǰ�ƹ��Ƿ�Ϊ��ı�־λ�����ڷ�ֹ��ť������С��Χȴδ�ر�ʱ��
// �ظ����빦�ܴ����������µƹ���˸
// 0--��1--��
static volatile bit is_light_lowest_flag = 0;

void knob_config(void)
{
    // ��ť��/����������Ϊ��������
    p15_input_config();
    // ���ڼ����ťһ���ѹ��adc������
    adc_config();

    adc_getval();
    last_adc_val = adc_val;                 //  ��һ���ϵ�ʱ�����adcֵ����ֹ�ϵ�ʱ��ťû�йرգ����һ�ϵ�͵����ƣ�
    last_knob_on_off_pin = KNOB_ON_OFF_PIN; // ��һ���ϵ�ʱ����¼��ť��/�����ŵĵ�ƽ״̬
}

u16 __target_pwm_duty_update(u16 __target_pwm_duty)
{
    // �������Ҫ���ڵ���pwmռ�ձȣ���ȷ��С�����1λ
    // ���� target_pwm_duty = 254����ռ�ձ�Ϊ25.4%
    u16 target_pwm_duty = 0;

    adc_getval(); // �õ�һ���˲����adcֵ

    if ((1 == is_light_lowest_flag && adc_val <= (ADC_MIN_VAL + ADC_STEP_VAL)) ||
        (last_adc_val <= (adc_val + ADC_STEP_VAL) && (last_adc_val >= (adc_val - ADC_STEP_VAL))))
    {
        // �����õ���ֵ��Ӧ��ť����Сλ�ø�����ͬ���˳�����
        // �����ֵ�� �� ���ڣ�����Ϊû�б仯����������Ӧ�Ĵ������
        return __target_pwm_duty; // ����ԭ������ֵ
    }

    last_adc_val = adc_val; //  ��¼֮ǰ����adcֵ

    // �õ���ǰadcֵռ�ܵ�ѹֵ�İٷֱȣ���ȷ��С�����1λ
    // ����ǰ��⵽�ĵ�ѹֵռ�ܵ�ѹֵ�İٷֱ� �պö�Ӧ ���ȵİٷֱȣ�
    target_pwm_duty = (u32)adc_val * 700 / ADC_MAX_VAL;

    // ��������Ŀ��ռ�ձȳ����˵�ǰ֧�ֵ����ռ�ձȣ�������ռ�ձȴ���
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

    if (adc_val <= ADC_MIN_VAL) // adcֵС���趨����Сֵ���䣬һ�ɰ���1.0%��ռ�ձ�������
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

// ��ť��صĴ�����
void knob_handle(void)
{
    u8 i = 0;

    // �������Ҫ���ڵ���pwmռ�ձȣ���ȷ��С�����1λ
    // ���� target_pwm_duty = 254����ռ�ձ�Ϊ25.4%
    u16 target_pwm_duty = 0;

    // if (LIGHT_CTL_MODE_MUILTY == light_ctl_mode && LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status)
    // {
    //     // �����ǰ������ɫ�л�ģʽ�еĹص�״̬����ͨ����ť���ڵƹ����ȣ�ֱ�������л��˵ƹ���ɫ
    //     return;
    // }

    // ��ť��/������Ϊ�͵�ƽ�����ܽ��к�����adc���ʹ������
    if (KNOB_ON_OFF_PIN && 1 == last_knob_on_off_pin) // ���Ϊ�ߵ�ƽ��ֱ�ӷ��أ������ad
    {
        return;
    }
    else if (1 == KNOB_ON_OFF_PIN && 0 == last_knob_on_off_pin)
    {
        delay_ms(10); // ��ť���ػ��ж���������Ҫ��ʱ����
        if (!(1 == KNOB_ON_OFF_PIN && 0 == last_knob_on_off_pin))
        {
            return;
        }

        // ���֮ǰΪ�͵�ƽ�����ڼ�⵽�˸ߵ�ƽ��˵��Ҫִ�йصƲ���
        // �صƺ�������Ӧ�ı�־λ�������˳�
        last_knob_on_off_pin = KNOB_ON_OFF_PIN; // �������ڼ�¼�ı�־λ��״̬

        // ���ܴ���ʲôģʽ��ֱ�ӹر���������
        cur_pwm_duty = 0;
        tmr0_set_pwm_duty(cur_pwm_duty);
        tmr1_set_pwm_duty(cur_pwm_duty);
        yellow_light_close();
        white_light_close();
        light_on_off_status = 0;  // ���µƹ�״̬
        is_light_lowest_flag = 0; // �����־λ��״̬���ƹ�ر�֮�󣬲���Ϊ������С���ȣ�

        // �´γ���Ҫ����������ȣ�������������ع����л����Ӧ�ĵƣ�
        light_adjust_add_sub_flag = 1; // ���±�־λ
        send_key_status(0x00);         // ���Ͳ���������Ϣ�����ݣ���Ҫ�Ƿ��͵�ǰ�ĵƹ�״̬
        return;
    }
    else if (0 == KNOB_ON_OFF_PIN && 1 == last_knob_on_off_pin)
    {
        delay_ms(10); // ��ť���ػ��ж���������Ҫ��ʱ����
        if (!(0 == KNOB_ON_OFF_PIN && 1 == last_knob_on_off_pin))
        {
            return;
        }
        // ���֮ǰΪ�ߵ�ƽ������Ϊ�͵�ƽ��˵��Ҫִ�п��Ʋ�����������PWM

        if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode ||
            LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status)
        {
            // ����ǰ׵�
            white_light_open();
        }
        else if (LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status)
        {
            // ����ǻƵ�
            yellow_light_open();
        }
        else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
        {
            // ����ǻư׵�
            white_light_open();
            yellow_light_open();
        }
        else if (LIGHT_MUILTY_CUR_OFF == light_muilty_cur_status) // �׵ƣ��ӹصƵ�״̬->�׵ƣ�
        {
            cur_pwm_duty = cur_white_duty;
            tmr1_set_pwm_duty(cur_pwm_duty); // �׵�
            white_light_open();              // ��������IO�Ͷ�ʱ��

            light_muilty_cur_status = LIGHT_MUILTY_CUR_WHITE;
            // light_on_off_status = 1; // ���µƵ�״̬
            // send_key_status(0x00);   // ���Ͳ���������Ϣ�����ݣ���Ҫ�Ƿ��͵�ǰ�ĵƹ�״̬���ӹص�״̬��ɰ׵ƣ�Ҫ���͸�״̬��

            if (cur_pwm_duty <= PWM_MIN_DUTY_LIGHT)
            {
                // �´γ���Ҫ�����������
                light_adjust_add_sub_flag = 1; // �������ڿ��Ƶ����ƹ����ȵı�־λ
            }
            else
            {
                // �´γ���Ҫ��ɼ�С����
                light_adjust_add_sub_flag = 0; // �������ڿ��Ƶ����ƹ����ȵı�־λ
            }
        }

        light_on_off_status = 1; // ���µƹ�״̬
        send_key_status(0x00);   // ���Ͳ���������Ϣ�����ݣ���Ҫ�Ƿ��͵�ǰ�ĵƹ�״̬
    }

    last_knob_on_off_pin = KNOB_ON_OFF_PIN;

    // ���Ϊ�͵�ƽ������������ִ�к����Ĳ���
    adc_getval(); // �õ�һ���˲����adcֵ

    // �������ﻹû�п��ǵ�һ���ϵ������������һ���ϵ磬��ť��λ�ò��̶����жϾͻ�������
    // Ҳû�п��ǵ��ڵ�������������ᵼ�µ�����������
    // û�п��ǵ�����ť������С��λ�ø��������
    // if (last_adc_val <= (adc_val + 10) && last_adc_val >= (adc_val - 10)) // ������ťû�ж�����Ҳ���д�������������
    // if (last_adc_val <= (adc_val + 40) && last_adc_val >= (adc_val - 40)) // ������ťû�ж�����Ҳ���д�������������
    // if (last_adc_val <= (adc_val + 50) && last_adc_val >= (adc_val - 50)) //
    if ((1 == is_light_lowest_flag && adc_val <= (ADC_MIN_VAL + ADC_STEP_VAL)) ||
        (last_adc_val <= (adc_val + ADC_STEP_VAL) && (last_adc_val >= (adc_val - ADC_STEP_VAL))))
    {
        // �����õ���ֵ��Ӧ��ť����Сλ�ø�����ͬ���˳�����
        // �����ֵ�� �� ���ڣ�����Ϊû�б仯����������Ӧ�Ĵ������
        return;
    }

    last_adc_val = adc_val; //  ��¼֮ǰ����adcֵ

    // �õ���ǰadcֵռ�ܵ�ѹֵ�İٷֱȣ���ȷ��С�����1λ
    // ����ǰ��⵽�ĵ�ѹֵռ�ܵ�ѹֵ�İٷֱ� �պö�Ӧ ���ȵİٷֱȣ�
    target_pwm_duty = (u32)adc_val * 700 / ADC_MAX_VAL;

    // ��������Ŀ��ռ�ձȳ����˵�ǰ֧�ֵ����ռ�ձȣ�������ռ�ձȴ���
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

    if (adc_val <= ADC_MIN_VAL) // adcֵС���趨����Сֵ���䣬һ�ɰ���1.0%��ռ�ձ�������
    // if (target_pwm_duty <= 30) // ���Ŀ��ռ�ձ�С��3.0%������1.0%������
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
    printf("��õ�adcֵ %d\n", adc_val);
    printf("Ŀ��ռ�ձ� %d\n", target_pwm_duty);
#endif // USE_MY_DEBUG

    while (1)
    {
        target_pwm_duty = __target_pwm_duty_update(target_pwm_duty);

        if (LIGHT_CTL_MODE_SINGLE == light_ctl_mode)
        {
            // �����ǰ�ǵ�ɫģʽ��ֻ���ư׵ƣ�
            if (0 == light_on_off_status)
            {
                // ���û�п��ƣ���ֱ���˳�
                return;
            }

            if (cur_pwm_duty < target_pwm_duty)
            {
                // �����Ҫ��������

                if (0 == light_on_off_status)
                {
                    // �����ʱ�ƹ�����ģ��򿪵ƹ⣬������״̬
                    tmr1_set_pwm_duty(cur_pwm_duty);
                    white_light_open();

                    light_on_off_status = 1; // ���µƹ�״̬
                    send_key_status(0x00);   // ���Ͳ���������Ϣ�����ݣ���Ҫ�Ƿ��͵�ǰ�ĵƹ�״̬
                }

                tmr1_set_pwm_duty(++cur_pwm_duty);
                cur_white_duty = cur_pwm_duty; // ��¼�׵Ƶ�pwm
                // �´γ���Ҫ��ɼ�С���ȣ�������������ع����л����Ӧ�ĵƣ�
                light_adjust_add_sub_flag = 0; // ���±�־λ
            }
            else if (cur_pwm_duty > target_pwm_duty)
            {
                // �����Ҫ��С����
                tmr1_set_pwm_duty(--cur_pwm_duty);
                cur_white_duty = cur_pwm_duty; // ��¼�׵Ƶ�pwm
                // �´γ���Ҫ����������ȣ�������������ع����л����Ӧ�ĵƣ�
                light_adjust_add_sub_flag = 1; // ���±�־λ
            }
            else if (cur_pwm_duty == target_pwm_duty)
            {
                // ����Ѿ����˶�Ӧ������
#if USE_MY_DEBUG
                printf("��ǰռ�ձȣ� %d\n", target_pwm_duty);
#endif
                break; // �˳�����
            }

            light_on_off_status = 1; // ���µƹ�״̬
        }
        else if (LIGHT_CTL_MODE_MUILTY == light_ctl_mode)
        {
            // �����ǰ����ɫ�л�ģʽ
            // �жϵ�ǰ�ǲ��ǻư�ɫ������ǣ���Ҫע������ռ�ձ�Ϊ60%��������ǣ�����ռ�ձ�Ϊ70%
            if (LIGHT_MUILTY_CUR_WHITE == light_muilty_cur_status) // ��ǰΪ��ɫ��
            {
                // �����ǰ������������
                if (cur_pwm_duty < target_pwm_duty)
                {
                    // if (0 == light_on_off_status)
                    // {
                    //     // �����ʱ�ƹ�����ģ��򿪵ƹ⣬������״̬
                    //     tmr1_set_pwm_duty(cur_pwm_duty);
                    //     white_light_open();

                    //     light_on_off_status = 1; // ���µƹ�״̬
                    //     send_key_status(0x00);   // ���Ͳ���������Ϣ�����ݣ���Ҫ�Ƿ��͵�ǰ�ĵƹ�״̬
                    // }

                    tmr1_set_pwm_duty(++cur_pwm_duty);
                    cur_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                    // �´γ���Ҫ��ɼ�С���ȣ�������������ع����л����Ӧ�ĵƣ�
                    light_adjust_add_sub_flag = 0; // ���±�־λ
                }
                else if (cur_pwm_duty > target_pwm_duty)
                {
                    // �����Ҫ��С����
                    tmr1_set_pwm_duty(--cur_pwm_duty);
                    cur_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                    // �´γ���Ҫ����������ȣ�������������ع����л����Ӧ�ĵƣ�
                    light_adjust_add_sub_flag = 1; // ���±�־λ
                }
                else
                {
                    // ����Ѿ����˶�Ӧ������
                    // tmr1_set_pwm_duty(cur_pwm_duty);
                    break;
                }
            }
            else if (LIGHT_MUILTY_CUR_YELLOW == light_muilty_cur_status) // �����ǰ�ǻƵ�
            {
                // �����ǰ������������
                if (cur_pwm_duty < target_pwm_duty)
                {
                    // if (0 == light_on_off_status)
                    // {
                    //     // �����ʱ�ƹ�����ģ��򿪵ƹ⣬������״̬
                    //     tmr0_set_pwm_duty(cur_pwm_duty);
                    //     yellow_light_open();

                    //     light_on_off_status = 1; // ���µƹ�״̬
                    //     send_key_status(0x00);   // ���Ͳ���������Ϣ�����ݣ���Ҫ�Ƿ��͵�ǰ�ĵƹ�״̬
                    // }

                    tmr0_set_pwm_duty(++cur_pwm_duty);
                    cur_yellow_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                    // �´γ���Ҫ��ɼ�С���ȣ�������������ع����л����Ӧ�ĵƣ�
                    light_adjust_add_sub_flag = 0; // ���±�־λ
                }
                else if (cur_pwm_duty > target_pwm_duty)
                {
                    // �����Ҫ��С����
                    tmr0_set_pwm_duty(--cur_pwm_duty);
                    cur_yellow_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                    // �´γ���Ҫ����������ȣ�������������ع����л����Ӧ�ĵƣ�
                    light_adjust_add_sub_flag = 1; // ���±�־λ
                }
                else
                {
                    // ����Ѿ������趨������
                    // tmr0_set_pwm_duty(cur_pwm_duty);
                    break;
                }
            }
            else if (LIGHT_MUILTY_CUR_YELLOW_WHITE == light_muilty_cur_status)
            {
                // �����ǰ�ǻư�ɫ

                // �����ǰ������������
                if (cur_pwm_duty < target_pwm_duty)
                {
                    // if (0 == light_on_off_status)
                    // {
                    //     // �����ʱ�ƹ�����ģ��򿪵ƹ⣬������״̬
                    //     tmr0_set_pwm_duty(cur_pwm_duty);
                    //     tmr1_set_pwm_duty(cur_pwm_duty);
                    //     yellow_light_open();
                    //     white_light_open();

                    //     light_on_off_status = 1; // ���µƹ�״̬
                    //     send_key_status(0x00);   // ���Ͳ���������Ϣ�����ݣ���Ҫ�Ƿ��͵�ǰ�ĵƹ�״̬
                    // }

                    tmr0_set_pwm_duty(++cur_pwm_duty);
                    tmr1_set_pwm_duty(cur_pwm_duty);
                    cur_yellow_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                    // �´γ���Ҫ��ɼ�С���ȣ�������������ع����л����Ӧ�ĵƣ�
                    light_adjust_add_sub_flag = 0; // ���±�־λ
                }
                else if (cur_pwm_duty > target_pwm_duty)
                {
                    // �����ǰ���ڼ�С����
                    tmr0_set_pwm_duty(--cur_pwm_duty);
                    tmr1_set_pwm_duty(cur_pwm_duty);

                    cur_yellow_white_duty = cur_pwm_duty; // ���¶��������PWMռ�ձ�
                    // �´γ���Ҫ����������ȣ�������������ع����л����Ӧ�ĵƣ�
                    light_adjust_add_sub_flag = 1; // ���±�־λ
                }
                else
                {
                    // ����Ѿ������������
                    break;
                }
            }
            else
            {
                break; // ������ǰ�ɫ�ƹ⡢��ɫ�ƹ�ͻư�ɫ�ƹ⣬ֱ�ӷ���
            }
        } // �����ǰ����ɫ�л�ģʽ

        // delay_100us(15); // ÿ1.5ms����0.1%��ռ�ձ�
        delay_100us(PWM_ADJUST_DUTY_TIME_STEP); // ÿx.xms����0.1%��ռ�ձ�
    } // while(1)
}

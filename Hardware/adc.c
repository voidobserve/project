// encoding GB2312
#include "adc.h"

volatile u16 adc_val = 0; // adcֵ��0~4095

// adc����
void adc_config(void)
{
    // P16--14��
    P1_MD1 |= GPIO_P16_MODE_SEL(0x3); // ģ��ģʽ

    // ADC����
    ADC_ACON1 &= ~(ADC_VREF_SEL(0x7) | ADC_EXREF_SEL(0x1)); // �����ѹѡ�񣬹ر��ⲿ�ο���ѹ
    ADC_ACON1 &= ~(ADC_INREF_SEL(0x01));                    // �ر��ڲ��ο���ѹ
    ADC_ACON1 |= ADC_VREF_SEL(0x06) | ADC_TEN_SEL(0x3);     // ѡ���ڲ��ο���ѹVCCA���رղ����ź�

    ADC_ACON0 = ADC_CMP_EN(0x1) |  // ��ADC�е�CMPʹ���ź�
                ADC_BIAS_EN(0x1) | // ��ADCƫ�õ�����ʹ�ź�
                ADC_BIAS_SEL(0x1); // ƫ�õ���ѡ��1x

    // P16 ͨ·
    ADC_CHS0 = ADC_ANALOG_CHAN(0x0E) |
               ADC_EXT_SEL(0x0); // ѡ���ⲿͨ·��������Ϊͨ����

    ADC_CFG0 |= ADC_CHAN0_EN(0x1) | // ʹ��ͨ��0ת��
                ADC_EN(0x1);        // ʹ��A/Dת��
    delay_ms(1);                    // �ȴ�ADCģ�������ȶ�����Ҫ�ȴ�20us����
}

// ��ȡadcֵ����ŵ�����adc_val��
u16 __adc_getval(void)
{
    ADC_CFG0 |= ADC_CHAN0_TRG(0x1); // ����ADC0ת��
    while (!(ADC_STA & ADC_CHAN0_DONE(0x1)))
        ;                                           // �ȴ�ת�����
    ADC_STA = ADC_CHAN0_DONE(0x1);                  // ���ADC0ת����ɱ�־λ
    return ((ADC_DATAH0 << 4) | (ADC_DATAL0 >> 4)); // ��ȡADC0��ֵ
}

// �ɼ����adcֵ�����յõ��˲����adcֵ
void adc_getval(void)
{
    // u8 i = 0;
    // u32 tmp = 0;
    // for (i = 0; i < 32; i++)
    // {
    //     tmp += __adc_getval();

    //     if (i >= 1)
    //     {
    //         tmp >>= 1; // �൱��tmp /= 2;
    //     }
    // }

    // adc_val = (u16)tmp;

    u8 cnt = 0; // adc�ɼ������ļ���
    u32 g_temp_value = 0;
    u32 g_tmpbuff = 0;
    u32 g_adcmax = 0;
    u32 g_adcmin = 0xFFFF;

    for (cnt = 0; cnt < 20; cnt++)
    {
        g_temp_value = __adc_getval();
        if (cnt < 2)
            continue; // ����ǰ���β�����
        if (g_temp_value > g_adcmax)
            g_adcmax = g_temp_value; // ���
        if (g_temp_value < g_adcmin)
            g_adcmin = g_temp_value; // ��С
        g_tmpbuff += g_temp_value;
    }
    g_tmpbuff -= g_adcmax;           // ȥ��һ�����
    g_tmpbuff -= g_adcmin;           // ȥ��һ����С
    g_temp_value = (g_tmpbuff >> 4); // ����16��ȡƽ��ֵ

    adc_val = g_temp_value;
}

#if USE_MY_DEBUG
void adc_showval(void)
{
    // ���Ӵ��ڲ鿴ADCֵ
    printf("adc_val = %d\r\n", adc_val);
}
#endif // USE_MY_DEBUG

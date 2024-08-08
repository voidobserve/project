// encoding GB2312
#include "adc.h"

volatile u16 adc_val = 0; // adc值，0~4095

// adc配置
void adc_config(void)
{
    // P16--14脚
    P1_MD1 |= GPIO_P16_MODE_SEL(0x3); // 模拟模式

    // ADC配置
    ADC_ACON1 &= ~(ADC_VREF_SEL(0x7) | ADC_EXREF_SEL(0x1)); // 清除电压选择，关闭外部参考电压
    ADC_ACON1 &= ~(ADC_INREF_SEL(0x01));                    // 关闭内部参考电压
    ADC_ACON1 |= ADC_VREF_SEL(0x06) | ADC_TEN_SEL(0x3);     // 选择内部参考电压VCCA，关闭测试信号

    ADC_ACON0 = ADC_CMP_EN(0x1) |  // 打开ADC中的CMP使能信号
                ADC_BIAS_EN(0x1) | // 打开ADC偏置电流能使信号
                ADC_BIAS_SEL(0x1); // 偏置电流选择：1x

    // P16 通路
    ADC_CHS0 = ADC_ANALOG_CHAN(0x0E) |
               ADC_EXT_SEL(0x0); // 选择外部通路（引脚作为通道）

    ADC_CFG0 |= ADC_CHAN0_EN(0x1) | // 使能通道0转换
                ADC_EN(0x1);        // 使能A/D转换
    delay_ms(1);                    // 等待ADC模块配置稳定，需要等待20us以上
}

// 获取adc值，存放到变量adc_val中
u16 __adc_getval(void)
{
    ADC_CFG0 |= ADC_CHAN0_TRG(0x1); // 触发ADC0转换
    while (!(ADC_STA & ADC_CHAN0_DONE(0x1)))
        ;                                           // 等待转换完成
    ADC_STA = ADC_CHAN0_DONE(0x1);                  // 清除ADC0转换完成标志位
    return ((ADC_DATAH0 << 4) | (ADC_DATAL0 >> 4)); // 读取ADC0的值
}

// 采集多次adc值，最终得到滤波后的adc值
void adc_getval(void)
{
    // u8 i = 0;
    // u32 tmp = 0;
    // for (i = 0; i < 32; i++)
    // {
    //     tmp += __adc_getval();

    //     if (i >= 1)
    //     {
    //         tmp >>= 1; // 相当于tmp /= 2;
    //     }
    // }

    // adc_val = (u16)tmp;

    u8 cnt = 0; // adc采集次数的计数
    u32 g_temp_value = 0;
    u32 g_tmpbuff = 0;
    u32 g_adcmax = 0;
    u32 g_adcmin = 0xFFFF;

    for (cnt = 0; cnt < 20; cnt++)
    {
        g_temp_value = __adc_getval();
        if (cnt < 2)
            continue; // 丢弃前两次采样的
        if (g_temp_value > g_adcmax)
            g_adcmax = g_temp_value; // 最大
        if (g_temp_value < g_adcmin)
            g_adcmin = g_temp_value; // 最小
        g_tmpbuff += g_temp_value;
    }
    g_tmpbuff -= g_adcmax;           // 去掉一个最大
    g_tmpbuff -= g_adcmin;           // 去掉一个最小
    g_temp_value = (g_tmpbuff >> 4); // 除以16，取平均值

    adc_val = g_temp_value;
}

#if USE_MY_DEBUG
void adc_showval(void)
{
    // 连接串口查看ADC值
    printf("adc_val = %d\r\n", adc_val);
}
#endif // USE_MY_DEBUG

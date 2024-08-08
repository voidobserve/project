/**
  ******************************************************************************
  * @file    adc.h
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    01-05-2021
  * @brief   This file contains all the ADC LL firmware functions.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
  *
  *
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|            |  7  | 开启硬件校准,开启后的通道0作为校准结果,填回去comp_trim_vdd           |
|            |  6  | A/D转换使能(ADC使能)  0x0:关闭  0x1:使能                             |
|            |  5  | 通道2转换使能   0x0:关闭  0x1:使能                                   |
|            |  4  | 通道1转换使能   0x0:关闭  0x1:使能                                   |
|  ADC_CFG0  |  3  | 通道0转换使能   0x0:关闭  0x1:使能                                   |
|            |  2  | ADC通道2触发转换  写1开始触发通道3进行转换                           |
|            |  1  | ADC通道1触发转换  写1开始触发通道1进行转换                           |
|            |  0  | ADC通道0触发转换  写1开始触发通道0进行转换                           |
|            |----------------------------------------------------------------------------|
|            |     | Note:dly功能没有带仲裁功能,所以在使用DLY功能的时候不能同时使用两个   |
|            |     |      通道进行转换                                                    |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_CFG0
#define _ADC_CFG0
#define ADC_CALIB_EN(n)            ((n) << 7)  // 开启硬件校准,开启后的通道0作为校准结果,填回去comp_trim_vdd
#define ADC_EN(n)                  ((n) << 6)  // A/D转换使能(ADC使能)  0x0:关闭  0x1:使能
#define ADC_CHAN2_EN(n)            ((n) << 5)  // 通道2转换使能   0x0:关闭  0x1:使能
#define ADC_CHAN1_EN(n)            ((n) << 4)  // 通道1转换使能   0x0:关闭  0x1:使能
#define ADC_CHAN0_EN(n)            ((n) << 3)  // 通道0转换使能   0x0:关闭  0x1:使能
#define ADC_CHAN2_TRG(n)           ((n) << 2)  // ADC通道2触发转换  写1开始触发通道3进行转换
#define ADC_CHAN1_TRG(n)           ((n) << 1)  // ADC通道1触发转换  写1开始触发通道1进行转换
#define ADC_CHAN0_TRG(n)           ((n) << 0)  // ADC通道0触发转换  写1开始触发通道0进行转换

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|            |  7  | 写1触发数字校准功能                                                  |
|  ADC_CFG1  | 6:3 | ADC时钟分频,分频比为n+1,3分频以下无法正常工作,需要配置大于3分频      |
|            |  2  | ADC通道2的中断使能  0x0:中断不使能  0x1:中断使能                     |
|            |  1  | ADC通道1的中断使能  0x0:中断不使能  0x1:中断使能                     |
|            |  0  | ADC通道0的中断使能  0x0:中断不使能  0x1:中断使能                     |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_CFG1
#define _ADC_CFG1
#define ADC_CP_CALIB_TRG(n)        ((n) << 7)  // 写1触发数字校准功能
#define ADC_CLK_SET(n)             ((n) << 3)  // ADC时钟分频,分频比为n+1,3分频以下无法正常工作,需要配置大于3分频
#define ADC_CHAN2_IRQ_EN(n)        ((n) << 2)  // ADC通道2的中断使能  0x0:中断不使能  0x1:中断使能
#define ADC_CHAN1_IRQ_EN(n)        ((n) << 1)  // ADC通道1的中断使能  0x0:中断不使能  0x1:中断使能
#define ADC_CHAN0_IRQ_EN(n)        ((n) << 0)  // ADC通道0的中断使能  0x0:中断不使能  0x1:中断使能

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|  ADC_CFG2  | 7:0 | 通道0采样时间配置,配置比为n+1时钟                                    |
|            |----------------------------------------------------------------------------|
|            |     | Note:为了ADC能够正常工作,采样时间最少配置为4                         |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_CFG2
#define _ADC_CFG2
#define ADC_CHAN0_SMP_TIME(n)        ((n) << 0)  // 通道0采样时间配置,配置比为n+1时钟
                                                 // Note:为了ADC能够正常工作,采样时间最少配置为4

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|  ADC_CFG3  | 7:0 | 通道1采样时间配置,配置比为n+1时钟                                    |
|            |----------------------------------------------------------------------------|
|            |     | Note:为了ADC能够正常工作,采样时间最少配置为4                         |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_CFG3
#define _ADC_CFG3
#define ADC_CHAN1_SMP_TIME(n)        ((n) << 0)  // 通道1采样时间配置,配置比为n+1时钟
                                                 // Note:为了ADC能够正常工作,采样时间最少配置为4

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|  ADC_CFG4  | 7:0 | 通道2采样时间配置,配置比为n+1时钟                                    |
|            |----------------------------------------------------------------------------|
|            |     | Note:为了ADC能够正常工作,采样时间最少配置为4                         |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_CFG4
#define _ADC_CFG4
#define ADC_CHAN2_SMP_TIME(n)        ((n) << 0)  // 通道2采样时间配置,配置比为n+1时钟
                                                 // Note:为了ADC能够正常工作,采样时间最少配置为4

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|            |  7  | ADC中模拟比较器输出,用于校准                                         |
|            |  6  | ADC刹车信号标志,写1清0                                               |
|            |  5  | 数字校准完成标志,写1清0                                              |
|            |  4  | 模拟校准完成标志,写1清0                                              |
|   ADC_STA  |  3  | 通道2转换结束,写1清0                                                 |
|            |  2  | 通道1转换结束,写1清0                                                 |
|            |  1  | 通道0转换结束,写1清0                                                 |
|            |  0  | ADC忙/空闲  0x0:ADC空闲  0x1:ADC转换中                               |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_STA
#define _ADC_STA
#define ADC_CMP_OUT(n)              ((n) << 7)  // ADC中模拟比较器输出,用于校准
#define ADC_BRK_SIGN_FLAG(n)        ((n) << 6)  // ADC刹车信号标志,写1清0
#define ADC_DIG_CALIB_DONE(n)       ((n) << 5)  // 数字校准完成标志,写1清0
#define ADC_ANA_CALIB_DONE(n)       ((n) << 4)  // 模拟校准完成标志,写1清0
#define ADC_CHAN2_DONE(n)           ((n) << 3)  // 通道2转换结束,写1清0
#define ADC_CHAN1_DONE(n)           ((n) << 2)  // 通道1转换结束,写1清0
#define ADC_CHAN0_DONE(n)           ((n) << 1)  // 通道0转换结束,写1清0
#define ADC_BUSY(n)                 ((n) << 0)  // ADC忙/空闲  0x0:ADC空闲  0x1:ADC转换中

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
| ADC_DATAH0 | 7:0 | 通道0的12位A/D转换结果高8位                                          |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_DATAH0
#define _ADC_DATAH0
#define ADC_CHAN0_DATAH(n)        ((n) << 0)  // 通道0的12位A/D转换结果高8位

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
| ADC_DATAL0 | 7:4 | 通道0的12位A/D转换结果低4位                                          |
|            | 3:0 |                                                                      |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_DATAL0
#define _ADC_DATAL0
#define ADC_CHAN0_DATAL(n)        ((n) << 4)  // 通道0的12位A/D转换结果低4位

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
| ADC_DATAH1 | 7:0 | 通道1的12位A/D转换结果高8位                                          |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_DATAH1
#define _ADC_DATAH1
#define ADC_CHAN1_DATAH(n)        ((n) << 0)  // 通道1的12位A/D转换结果高8位

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
| ADC_DATAL1 | 7:4 | 通道1的12位A/D转换结果低4位                                          |
|            | 3:0 |                                                                      |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_DATAL1
#define _ADC_DATAL1
#define ADC_CHAN1_DATAL(n)        ((n) << 4)  // 通道1的12位A/D转换结果低4位

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
| ADC_DATAH2 | 7:0 | 通道2的12位A/D转换结果高8位                                          |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_DATAH2
#define _ADC_DATAH2
#define ADC_CHAN2_DATAH(n)        ((n) << 0)  // 通道2的12位A/D转换结果高8位

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
| ADC_DATAL2 | 7:4 | 通道2的12位A/D转换结果低4位                                          |
|            | 3:0 |                                                                      |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_DATAL2
#define _ADC_DATAL2
#define ADC_CHAN2_DATAL(n)        ((n) << 4)  // 通道2的12位A/D转换结果低4位

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|            |  7  | 通道0/1/2 数据格式控制  0x0:左对齐    0x1:右对齐                     |
|            |  6  | 通道0/1/2 DLY功能使能   0x0:关闭      0x1:使能                       |
|  ADC_CHS0/ |  5  | 内外部通道选择     0x0:外部通道  0x1:内部通道                        |
|  ADC_CHS1/ | 4:0 | 通道0/1/2 的模拟转换通道选择                                         |
|  ADC_CHS2  |     | 当ADC_EXT_SEL为0时,CHAN0/1/2的模拟通道选择为GPIO,对应关系为:         |
|            |     |      0x0:P00  0x1:P01  0x2:P02  ...  0x1A:P31                        |
|            |     | 当ADC_EXT_SEL为1时,CHAN0/1/2的模拟通道对应选择为内部通道,对应关系为: |
|            |     |      0x0:ATO   0x1:VREF_1P2  0x2:PTAT  0x3:VCCA_D5                   |
|            |     |      0x4:AMP0  0x5:AMP1      0x6:AMP2  0x7:不使能                    |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_CHS0/ADC_CHS1/ADC_CHS2
#define _ADC_CHS0
#define _ADC_CHS1
#define _ADC_CHS2
#define ADC_DATA_FORMAT(n)          ((n) << 7)  // 通道0/1/2 数据格式控制  0x0:左对齐    0x1:右对齐
#define ADC_DLY_EN(n)               ((n) << 6)  // 通道0/1/2 DLY功能使能   0x0:关闭      0x1:使能
#define ADC_EXT_SEL(n)              ((n) << 5)  // 内外部通道选择     0x0:外部通道  0x1:内部通道
#define ADC_ANALOG_CHAN(n)          ((n) << 0)  // 通道0/1/2 的模拟转换通道选择
                                                // 当ADC_EXT_SEL为0时,CHAN0/1/2的模拟通道选择为GPIO,对应关系为:
                                                //      0x0:P00  0x1:P01  0x2:P02  ...  0x1A:P31
                                                // 当ADC_EXT_SEL为1时,CHAN0/1/2的模拟通道对应选择为内部通道,对应关系为:
                                                //      0x0:ATO   0x1:VREF_1P2  0x2:PTAT  0x3:VCCA_D5
                                                //      0x4:AMP0  0x5:AMP1      0x6:AMP2  0x7:不使能

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|            | 7:4 | 通道0/1/2DLY的ADC时钟个数选择，配置为4n+1                            |
|            | 3:0 | 通道0/1/2硬件触发源选择                                              |
|            |     | 0x0:选择软件触发              0x1:SUPER TIMER0 PWM上升沿             |
| ADC_TRGS0/ |     | 0x2:SUPER TIMER0 PWM下降沿    0x3:SUPER TIMER0 周期点                |
| ADC_TRGS1/ |     | 0x4:SUPER TIMER0 零点         0x5:SUPER TIMER2 PWM上升沿             |
| ADC_TRGS2  |     | 0x6:SUPER TIMER2 PWM下降沿    0x7:SUPER TIMER2 周期点                |
|            |     | 0x8:SUPER TIMER2 零点         0x9:SUPER TIMER4 PWM上升沿             |
|            |     | 0xA:SUPER TIMER4 PWM下降沿    0xB:SUPER TIMER4 周期点                |
|            |     | 0xC:SUPER TIMER4 零点         0xD:SUPER TIMER0 C点                   |
|            |     | 0xE:GPIO外部触发              0xF:TIMER0降采样触发                   |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_TRGS0/ADC_TRGS1/ADC_TRGS2
#define _ADC_TRGS0
#define _ADC_TRGS1
#define _ADC_TRGS2
#define ADC_DLY_CYC(n)              ((n) << 4)  // 通道0/1/2DLY的ADC时钟个数选择，配置为4n+1
#define ADC_TRIGGER_SEL(n)          ((n) << 0)  // 通道0/1/2硬件触发源选择
                                                // 0x0:选择软件触发              0x1:SUPER TIMER0 PWM上升沿
                                                // 0x2:SUPER TIMER0 PWM下降沿    0x3:SUPER TIMER0 周期点
                                                // 0x4:SUPER TIMER0 零点         0x5:SUPER TIMER2 PWM上升沿
                                                // 0x6:SUPER TIMER2 PWM下降沿    0x7:SUPER TIMER2 周期点
                                                // 0x8:SUPER TIMER2 零点         0x9:SUPER TIMER4 PWM上升沿
                                                // 0xA:SUPER TIMER4 PWM下降沿    0xB:SUPER TIMER4 周期点
                                                // 0xC:SUPER TIMER4 零点         0xD:SUPER TIMER0 C点
                                                // 0xE:GPIO外部触发              0xF:TIMER0降采样触发

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|                 | Bit |                    Description                                  |
|                 |-----------------------------------------------------------------------|
|  ADC_CMPDATAH   | 7:0 | 数字比较器刹车的预设值，高8bit                                  |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_CMPDATAH
#define _ADC_CMPDATAH
#define ADC_CMP_DATAH(n)            ((n) << 0)  // 数字比较器刹车的预设值，高8bit

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|                 | Bit |                    Description                                  |
|                 |-----------------------------------------------------------------------|
|                 | 7:4 | 数字比较器刹车的预设值,低4bit                                   |
|                 |  3  | 加速模式,当通道0和通道1都选择同一个触发源的时候,硬件会自动切换  |
|  ADC_CMPDATAL   |     | 通道,提前保证通道的建立时间能够满足  0x0:关闭  0x1:使能         |
|                 |  2  | 通道2使能数字比较功能  0x0:关闭  0x1:使能                       |
|                 |  1  | 通道1使能数字比较功能  0x0:关闭  0x1:使能                       |
|                 |  0  | 通道0使能数字比较功能  0x0:关闭  0x1:使能                       |
|                 |-----------------------------------------------------------------------|
|                 |     | Note：只有一个预设值,所以当使能两个或以上的通道数字刹车时,      |
|                 |     | 只有一个预设值,所有通道共用一个预设值。                         |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_CMPDATAL
#define _ADC_CMPDATAL
#define ADC_CMP_DATAL(n)            ((n) << 4)  // 数字比较器刹车的预设值,低4bit
#define ADC_SPEED_MODE_EN(n)        ((n) << 3)  // 加速模式,当通道0和通道1都选择同一个触发源的时候,硬件会自动切换
                                                // 通道,提前保证通道的建立时间能够满足  0x0:关闭  0x1:使能
#define ADC_CHAN0_CMP_EN(n)         ((n) << 2)  // 通道2使能数字比较功能  0x0:关闭  0x1:使能
#define ADC_CHAN1_CMP_EN(n)         ((n) << 1)  // 通道1使能数字比较功能  0x0:关闭  0x1:使能
#define ADC_CHAN2_CMP_EN(n)         ((n) << 0)  // 通道0使能数字比较功能  0x0:关闭  0x1:使能

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|            |  7  |                                                                      |
|            | 6:4 | ADC内部检测信号选择  0x0:ATO   0x1:VREF_1P2  0x2:PTAT  0x3:VCCA_D5   |
|            |     |                      0x4:AMP0  0x5:AMP1      0x6:AMP2  0x7:不使能    |
| ADC_ACON0  | 3:2 | ADC 偏置电流选择位  <0>0=1.25X  1=1X                                 |
|            |     |                     <1>0:ref_inner=0.6V  1:ref_inner=1.05            |
|            |  1  | ADC 偏置电流能使信号  0x0:关闭  0x1:打开                             |
|            |  0  | ADC中CMP使能信号      0x0:关闭  0x1:打开                             |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_ACON0
#define _ADC_ACON0
#define ADC_DETCH_SEL(n)       ((n) << 4)      // ADC内部检测信号选择  0x0:ATO   0x1:VREF_1P2  0x2:PTAT  0x3:VCCA_D5
                                               //                      0x4:AMP0  0x5:AMP1      0x6:AMP2  0x7:不使能
#define ADC_BIAS_SEL(n)        ((n) << 2)      // ADC 偏置电流选择位  <0>0=1.25X  1=1X
                                               //                     <1>0:ref_inner=0.6V  1:ref_inner=1.05
#define ADC_BIAS_EN(n)         ((n) << 1)      // ADC 偏置电流能使信号  0x0:关闭  0x1:打开
#define ADC_CMP_EN(n)          ((n) << 0)      // ADC中CMP使能信号      0x0:关闭  0x1:打开

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|            |  7  | ADC 校准电流选择  0x0:1X  0x1:2X                                     |
|            |  6  | ADC中内部参考能使信号  0x0:关闭  0x1:打开                            |
|            |     | Note:选择内部参考时,必须先断开外部参考!                              |
| ADC_ACON1  |  5  | ADC外部参考选择信号  0x0:不选择外部参考  0x1:选择EXREF(P07)为参考电压 | 
|            |     | Note:选择外部参考时,必须选关闭内部参考!                              |
|            | 4:3 | ADC测试信号选择  0x0:VREFP  0x1:保留  0x2:保留  0x3:关闭测试信号     |
|            | 2:0 | ADC中内部参考电压选择信号                                            |
|            |     | 0x0:1.2V  0x1:2.0V  0x2:2.4V  0x3:3.0V  0x4:3.6V  0x5:4.2V           |
|            |     | 0x6:VCCA,此时需要配ADC_SELINREF=0,ADC_SELEXREF=0  0x7:保留位         |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_ACON1
#define _ADC_ACON1
#define ADC_TRIMIB_SEL(n)      ((n) << 7)      // ADC 校准电流选择  0x0:1X  0x1:2X
#define ADC_INREF_SEL(n)       ((n) << 6)      // ADC中内部参考能使信号  0x0:关闭  0x1:打开
                                               // Note:选择内部参考时,必须先断开外部参考!
#define ADC_EXREF_SEL(n)       ((n) << 5)      // ADC外部参考选择信号  0x0:不选择外部参考   0x1:选择EXREF(P07)为参考电压
                                               // Note:选择外部参考时,必须先关闭内部参考!
#define ADC_TEN_SEL(n)         ((n) << 3)      // ADC测试信号选择  0x0:VREFP  0x1:保留  0x2:保留  0x3:关闭测试信号
#define ADC_VREF_SEL(n)        ((n) << 0)      // ADC中内部参考电压选择信号
                                               // 0x0:1.2V  0x1:2.0V  0x2:2.4V  0x3:3.0V  0x4:3.6V  0x5:4.2V  
                                               // 0x6:VCCA,此时需要配ADC_SELINREF=0,ADC_SELEXREF=0  0x7:保留位

/******************************************************************************************
|-----------------------------------------------------------------------------------------|
|            | Bit |                    Description                                       |
|            |----------------------------------------------------------------------------|
|            |  7  | ADC 比较器偏置电流选择   0x0:1X    0x1:1.25X                         |
| ADC_ACON2  |  6  | ADC比较器校准功能使信号  0x0:关闭  0x1:打开                          |
|            | 5:0 | ADC比较器校准值配置位  MSB:符号位,低5为数值                          |
|-----------------------------------------------------------------------------------------|
******************************************************************************************/
// ADC_ACON2
#define _ADC_ACON2
#define ADC_CMP_BS_SEL(n)      ((n) << 7)      // ADC 比较器偏置电流选择   0x0:1X    0x1:1.25X
#define ADC_CMP_TRIM_EN(n)     ((n) << 6)      // ADC比较器校准功能使信号  0x0:关闭  0x1:打开
#define ADC_CMP_TRIM(n)        ((n) << 0)      // ADC比较器校准值配置位  MSB:符号位,低5为数值

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // __ADC_H__

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2021 HUGE-IC ***** END OF FILE *****/

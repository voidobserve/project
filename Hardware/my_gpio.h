// GPIO配置的头文件
#ifndef __MY_GPIO_H
#define __MY_GPIO_H

#include "include.h" // 包含芯片官方提供的头文件

// 开发板上与LED正极连接的引脚
#define LED6 P01
#define LED7 P26

// p11、p12在开发板上作为串口使用
void p11_output_config(void);
void p12_output_config(void); // P12配置为输出模式

void p01_output_config(void); // P01连接到了开发板的LED6的正极
void p24_output_config(void);
void p25_output_config(void);
void p26_output_config(void); // P26连接到了开发板的LED7的正极P26配置为输出模式

void p06_input_config(void); // p06配置成输入上拉模式
void p07_input_config(void); // p07配置成输入上拉模式

void p10_input_config(void); // p10配置成输入上拉模式
void p13_input_config(void); // p13配置成输入上拉模式
void p14_input_config(void); // p14配置成输入上拉模式
void p15_input_config(void); // p15配置成输入上拉模式
void p17_input_config(void); // p17配置成输入上拉模式

void p21_input_config(void); // p21配置成输入上拉模式
void p22_input_config(void); // p22配置成输入上拉模式
void p23_input_config(void); // p23配置成输入上拉模式
void p27_input_config(void); // p27配置成输入上拉模式

void p30_input_config(void); // p30配置成输入上拉模式
void p31_input_config(void); // p31配置成输入上拉模式

#endif

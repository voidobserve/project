// encode UTF-8
#ifndef __LIGHT_H
#define __LIGHT_H

#include "include.h"   // 包含芯片官方提供的头文件
#include "my_config.h" // 包含自定义的配置文件

void check_light_ctl_mode(void); // 检测灯光控制的模式
void white_light_pwm_config(void);

void yellow_light_close(void); // 关闭黄灯（不能通过直接设置占空比为0来关灯，实际的波形还是会有百分之零点几的分量）
void white_light_close(void);  // 关闭白灯（不能通过直接设置占空比为0来关灯，实际的波形还是会有百分之零点几的分量）

void yellow_light_open(void); // 开启黄灯（开启前需要注意先设置好PWM的占空比）
void white_light_open(void);  // 开启白灯（开启前需要注意先设置好PWM的占空比）

void yellow_white_light_open(void);  // 开启黄白灯（在双色模式下使用）
void yellow_white_light_close(void); // 关闭黄白灯（在双色模式下使用）

#endif

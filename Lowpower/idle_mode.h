#ifndef __IDLE_MODE_H
#define __IDLE_MODE_H

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件
#if USE_IDLE_MODE // 使用Idle待机模式

extern volatile bit idle_mode_state;

// 待机模式配置
// P25配置为唤醒引脚，上升沿唤醒
void idle_mode_config(void);

#endif // USE_IDLE_MODE 
#endif // __IDLE_MODE_H
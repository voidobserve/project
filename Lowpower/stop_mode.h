#ifndef __STOP_MODE_H
#define __STOP_MODE_H

#include "include.h"   // 使用芯片官方提供的头文件
#include "my_config.h" // 包含自定义的头文件
#if USE_STOP_MODE // 使用stop停止模式

void stop_mode_config(void);


#endif // USE_STOP_MODE
#endif

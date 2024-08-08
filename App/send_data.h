// encode UTF-8
// 单线发送数据的头文件
#ifndef __SEND_DATA_H
#define __SEND_DATA_H

#include "include.h" // 包含芯片官方提供的头文件
#include "my_config.h" // 包含自定义配置的头文件

void send_key_status(u8 key_index); // 通过串口发送按键状态

#endif

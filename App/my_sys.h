// encode UTF-8
#ifndef __MY_SYS_H
#define __MY_SYS_H

#include "include.h"   // 包含芯片官方提供的头文件
#include "my_config.h" // 包含自定义配置的头文件

#define PWM_MAX_DUTY_SINGLE_LIGHT 700      // 单色时，最高占空比为 70.0%
#define PWM_MAX_DUTY_MUILTY_LIGHT 600      // 黄白色时，最高占空比为 60.0%
#define PWM_MIN_DUTY_LIGHT 100             // 长按和旋钮调节灯光亮度时，最小亮度为 10.0%
#define PWM_MIN_DUTY_YELLOW_WHITE_LIGHT 50 // 双色下，最小亮度为 5%
#define PWM_ADJUST_DUTY_TIME_STEP 30       // 每次调节PWM的时间间隔（单位：100us）

#define KNOB_ON_OFF_PIN P15 // 检测旋钮是否开/关的引脚，0--开启，1--关闭

#define ADC_MAX_VAL 1900 // adc检测到的最大的adc值(这里要经过实际测试，填写相应的adc值，范围：0~4095，实际肯定没有4095)
#define ADC_MIN_VAL 100  // 设定的adc最小值，一旦小于等于这个值，则让灯光显示最低亮度
#define ADC_STEP_VAL 100 // 设定adc跳变的值，超过这个值才认为adc值有变化，防止检测的adc有跳变

// #define LIGHT_OPEN_CLOSE_TIME 700 // 开启/关闭的总时间（单位：ms）
// #define LIGTH_GRADIENT_TIME 1800  // 调节灯光亮度时，灯光渐变的总时间（单位：ms）

extern bit light_ctl_mode;                    // 灯光控制模式
extern volatile bit is_touch_key_light_press; // 灯光按键是否有按下的标志位
// extern volatile bit is_touch_key_light_hold;  // 灯光按键是否长按的标志位
extern bit light_on_off_status; // 指示灯状态，0--关闭，1--开启

extern volatile u8 light_control_flag; // 控制灯光的标志位

extern volatile u8 light_muilty_cur_status; // 三色切换模式中，当前灯光的状态

extern bit light_adjust_add_sub_flag; // 用于标志当前是在增大亮度还是减少亮度的标志位

extern u16 cur_pwm_duty; // 当前驱动灯光的占空比，精确到小数点后1位
// 独立保存的驱动灯光的占空比（默认为最亮），精确到小数点后1位
extern u16 cur_white_duty;
extern u16 cur_yellow_duty;
extern u16 cur_yellow_white_duty;

extern bit touch_ctl_enable;          // 触摸按键使能的标志位
extern u16 touch_ctl_enable_time_cnt; // 控制触摸按键多少时间后使能的标志位

// 按键标号的定义
enum
{
    TOUCH_KEY_BULETOOTH = 1, // 按键--蓝牙
    TOUCH_KEY_LIGHT,         // 按键--灯
    TOUCH_KEY_CLOCK,         // 按键--时钟
    TOUCH_KEY_POWER,         // 按键--电源
    TOUCH_KEY_SUB,           // 按键--减
    TOUCH_KEY_ADD            // 按键--加
};

// // 按键状态的定义
// enum
// {
//     TOUCH_KEY_LOOSE = 0,  // 按键松开
//     TOUCH_KEY_CLICK,      // 按键单击
//     TOUCH_KEY_LONG_PRESS, // 按键长按
//     TOUCH_KEY_HOLD        // 按键持续（长按后，仍未松开）
// };

// 定义灯光控制的模式，最后会给变量 light_ctl_mode 赋值
// 灯光控制的模式只在上电检测一次，后续就根据该检测结果执行相应的灯光控制模式
enum
{
    LIGHT_CTL_MODE_SINGLE = 1, // 单色模式（只控制白灯）
    LIGHT_CTL_MODE_MUILTY = 0  // 三色切换模式（白->黄->黄白->关灯->白，如此循环）
};

// 控制灯光的标志定义，用于给变量 light_control_flag 赋值
enum
{
    LIGHT_CONTROL_NONE = 0, // 无控制

    LIGHT_YELLOW_CLOSE = 1, // 关闭黄灯
    LIGHT_YELLOW_OPEN,      // 开启黄灯
    LIGHT_WHITE_CLOSE,      // 关闭白灯
    LIGHT_WHITE_OPEN,       // 开启白灯

    LIGHT_YELLOW_WHITE_CLOSE, // 关闭黄白灯
    LIGHT_YELLOW_WHITE_OPEN,  // 开启黄白灯

    LIGHT_ADJUST_YELLOW,      // 调整黄灯亮度
    LIGHT_ADJUST_WHITE,       // 调整白灯亮度
    LIGHT_ADJUST_YELLOW_WHITE // 调整黄白灯亮度
};

// 三色切换模式中，当前灯光状态的定义，用于给变量 light_muilty_cur_status 赋值
enum
{
    LIGHT_MUILTY_CUR_WHITE,        // 当前显示的是白灯
    LIGHT_MUILTY_CUR_YELLOW,       // 当前显示的是黄灯
    LIGHT_MUILTY_CUR_YELLOW_WHITE, // 当前显示的是黄白灯
    LIGHT_MUILTY_CUR_OFF           // 当前灯的状态为关闭
};

#endif

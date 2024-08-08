// encode UTF-8
#include "my_sys.h"

// 灯光控制模式
// 由上电时对黄色灯的引脚检测而得
// 若检测到高电平，则接下来触摸按键可以控制开关灯（light_ctl_mode = 1）
// 若检测到低电平，则接下来触摸按键不仅可以控制开关灯，还可以控制三种颜色灯光的切换（light_ctl_mode = 0）
bit light_ctl_mode = 0;

// 灯光按键是否有按下的标志位
//  0--未按下，1--按下
volatile bit is_touch_key_light_press = 0; 

// 灯光按键是否长按的标志位
// 0--未长按，1--长按
// volatile bit is_touch_key_light_hold = 0;

// 电源指示灯状态，0--关闭，1--开启
bit light_on_off_status = 0;

// 控制灯光的标志位
volatile u8 light_control_flag = 0; 

// 三色切换模式中，当前灯光的状态
volatile u8 light_muilty_cur_status = LIGHT_MUILTY_CUR_OFF;

// 用于标志当前是在增大亮度还是减少亮度的标志位
// 0--减少亮度，1--增大亮度
bit light_adjust_add_sub_flag = 0; // 最开始灯是最亮的，应该减小亮度

// 当前驱动灯光的占空比，精确到小数点后1位
// 例如 cur_pwm_duty = 254，则占空比为25.4%
u16 cur_pwm_duty = 0;

// 独立保存的驱动灯光的占空比（默认为最亮），精确到小数点后1位
u16 cur_white_duty = PWM_MAX_DUTY_SINGLE_LIGHT; 
u16 cur_yellow_duty = PWM_MAX_DUTY_SINGLE_LIGHT;
u16 cur_yellow_white_duty = PWM_MAX_DUTY_MUILTY_LIGHT;
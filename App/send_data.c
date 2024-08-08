// encode UTF-8
// 单线发送数据的源文件
#include "send_data.h"

// void send_byte(u8 send_data)
// {
//     uart1_sendbyte(send_data);
// }

// 通过串口发送按键状态
// 参数1： key_index--按键序号
void send_key_status(u8 key_index)
{
    u8 tmp = 0;
    u16 send_datas = 0;
    tmp |= ((u8)light_on_off_status << 7) | (key_index << 4);
    send_datas |= tmp << 8;      // 高8位存放原码
    send_datas |= (~tmp & 0xFF); // 低8位存放反码
    uart1_sendbyte(0xA5);        // 发送格式头
    uart1_sendbyte((send_datas >> 8) & 0xFF);
    uart1_sendbyte(send_datas & 0xFF); // 发送反码
}

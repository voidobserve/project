#include "uart0.h"

#if USE_UART0

// #include <string.h> // memset()

// 设置的波特率需要适配单片机的时钟，这里直接使用了官方的代码
#define USER_UART0_BAUD ((SYSCLK - UART0_BAUDRATE) / (UART0_BAUDRATE))

// // 用来存放接收的数据帧的缓冲区
// volatile u8 uart0_recv_buf[UART0_RXBUF_LEN] = {0};
// volatile u16 uart0_recv_cnt = 0;  // 接收缓冲区计数值
// volatile bit uart0_recv_flag = 0; // 接收完成一帧的标志位

/*
    // 使用示例
    if (uart0_recv_flag)
    {
        uart0_recv_flag = 0;
        printf("%s\n", uart0_recv_buf);
        memset(uart0_recv_buf, 0, UART0_RXBUF_LEN);
        uart0_recv_cnt = 0;
    }
*/

#if USE_PRINTF_UART0
// 重写putchar()函数
char putchar(char c)
{
    uart0_sendbyte(c);
    return c;
}
#endif // USE_PRINTF_UART0

#ifdef DEVELOPMENT_BOARD
// uart0初始化
// 波特率由宏 UART0_BAUDRATE 来决定
void uart0_config(void)
{
    P2_MD1 &= (~GPIO_P25_MODE_SEL(0x3));
    P2_MD1 |= GPIO_P25_MODE_SEL(0x1); // 输出模式
    FOUT_S25 |= GPIO_FOUT_UART0_TX;   // 配置P22为UART0_TX

    UART0_BAUD1 = (USER_UART0_BAUD >> 8) & 0xFF; // 配置波特率高八位
    UART0_BAUD0 = USER_UART0_BAUD & 0xFF;        // 配置波特率低八位
    UART0_CON0 = UART_STOP_BIT(0x0) |
                 UART_RX_IRQ_EN(0x1) |
                 UART_EN(0x1); // 8bit数据，1bit停止位，使能RX中断
}
#endif // DEVELOPMENT_BOARD

#ifdef CIRCUIT_BOARD
// uart0初始化
// 波特率由宏 UART0_BAUDRATE 来决定
void uart0_init(void)
{
    // 电路板上，P11为发送引脚，P12为接收引脚
    P1_MD0 &= (~GPIO_P11_MODE_SEL(0x3) | ~GPIO_P12_MODE_SEL(0x3));
    P1_MD0 |= GPIO_P11_MODE_SEL(0x1); // 输出模式
    FOUT_S11 |= GPIO_FOUT_UART0_TX;   // 配置P11为UART0_TX
    FIN_S7 |= GPIO_FIN_SEL_P12;       // 配置P12为UART0_RX

    __EnableIRQ(UART0_IRQn); // 打开UART模块中断
    IE_EA = 1;               // 打开总中断

    UART0_BAUD1 = (USER_UART0_BAUD >> 8) & 0xFF; // 配置波特率高八位
    UART0_BAUD0 = USER_UART0_BAUD & 0xFF;        // 配置波特率低八位
    UART0_CON0 = UART_STOP_BIT(0x0) |
                 UART_RX_IRQ_EN(0x1) |
                 UART_EN(0x1); // 8bit数据，1bit停止位，使能RX中断

    // memset(uart0_recv_buf, 0, sizeof(uart0_recv_buf));
}
#endif // DEVELOPMENT_BOARD

// UART0发送一个字节数据的函数
void uart0_sendbyte(u8 senddata)
{
    while (!(UART0_STA & UART_TX_DONE(0x01)))
        ;
    UART0_DATA = senddata;
    while (!(UART0_STA & UART_TX_DONE(0x01))) // 等待这次发送完成
        ;
}

// // UART0中断服务函数（接收中断）
// void UART0_IRQHandler(void) interrupt UART0_IRQn
// {
//     // 进入中断设置IP，不可删除
//     __IRQnIPnPush(UART0_IRQn);
//     // ---------------- 用户函数处理 -------------------
//     // RX接收完成中断
//     if (UART0_STA & UART_RX_DONE(0x1))
//     {
//         uart0_recv_buf[uart0_recv_cnt++] = UART0_DATA;
//         if (UART0_DATA == '\n')
//         {
//             uart0_recv_flag = 1;
//         }
//     }

//     // 退出中断设置IP，不可删除
//     __IRQnIPnPop(UART0_IRQn);
// }

#endif // USE_UART0

#include "idle_mode.h"
#if USE_IDLE_MODE

// 待机模式状态标志位（需要注意的是，任意中断都能唤醒待机模式，可能需要在每个中断做好标志位的处理）
volatile bit idle_mode_state = 0; 

#if 0 // 使用示例

if (idle_mode_state == 0)
{
    LED6 = ~LED6;
    idle_mode_state = 1;
    idle_mode_config();
}

// 当标志位为0时，发生中断
if (idle_mode_state == 0)
{
    printf("Idle out\n");
}

if (uart1_recv_flag)
{
    uart1_recv_flag = 0;
    printf("%s\n", uart1_recv_buf);
    memset(uart1_recv_buf, 0, UART1_RXBUF_LEN);
    uart1_recv_cnt = 0;
}

LED7 = ~LED7;
delay_ms(500);

#endif

// 待机模式配置
// P25配置为唤醒引脚，上升沿唤醒
void idle_mode_config(void)
{
#if MY_DEBUG
    printf("Idle in\n");
#endif

    // 可以不用配置唤醒通道，只要有中断，就可以唤醒待机模式
    // LP_WKPND = LP_WKUP_0_PCLR(0x1);  // 清除唤醒标志位
    // LP_WKCON = (LP_WKUP_0_EDG(0x0) | // 通道0高电平触发唤醒
    //             LP_WKUP_0_EN(0x1));  // 唤醒通道0使能

    // 进入待机模式
    // 进入待机模式后，程序会停在这里，直到触发中断才退出待机模式，程序继续运行
    LP_CON &= ~LP_IDLE_EN(0x1);
}

// P2中断服务函数
void P2_IRQHandler(void) interrupt P2_IRQn
{
    // P0_PND寄存器写任何值都会清标志位
    u8 p2_pnd = P2_PND;

    // 进入中断设置IP，不可删除
    __IRQnIPnPush(P2_IRQn);

    // ---------------- 用户函数处理 -------------------

    if (p2_pnd & GPIO_P25_IRQ_PNG(0x1))
    {
        idle_mode_state = 0;
    }

    P2_PND = p2_pnd; // 清P2中断标志位

    // -------------------------------------------------

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(P2_IRQn);
}

#endif // USE_IDLE_MODE
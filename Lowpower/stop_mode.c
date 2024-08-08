#include "stop_mode.h"

#if USE_STOP_MODE // 使用stop停止模式

void stop_mode_config(void)
{
    // P17是烧录引脚，没调用IO_MAP关闭烧录功能情况下需要将此引脚上拉
    P1_PU |= GPIO_P17_PULL_UP(0x1);

    // 配置P25为输入下拉
    P2_MD1 &= ~GPIO_P25_MODE_SEL(0x3); // 配置P25为输入模式
    P2_PD |= GPIO_P25_PULL_PD(0x1);    // 配置P25为下拉
    P2_IMK |= GPIO_P25_IRQ_MASK(0x1);  // 使能P25的中断
    P2_TRG1 &= ~GPIO_P25_TRG_SEL(0x3); // 配置P25的上升沿触发
    P2_TRG1 |= GPIO_P25_TRG_SEL(0x2);
    FIN_S10 = GPIO_FIN_SEL_P25; // 配置P25为通道0输入唤醒端口

    __SetIRQnIP(P2_IRQn, P2_IQn_CFG); // 设置中断优先级
    __EnableIRQ(P2_IRQn);             // 使能P2中断
    IE_EA = 1;                        // 使能总开关

    // LP_WKPND = LP_WKUP_0_PCLR(0x1);  // 清除唤醒标志位
    // LP_WKCON = (LP_WKUP_0_EDG(0x0) | // 通道0高电平触发唤醒
    //             LP_WKUP_0_EN(0x1));  // 唤醒通道0使能

    CLK_CON0 &= ~CLK_SYSCLK_SEL(0x3);  // 系统时钟选择rc64k
    CLK_ACON0 &= ~CLK_AIP_HRC_EN(0x1); // 关闭HRC时钟
    LP_CON |= LP_STOP_EN(0x1);         // 进入Stop低功耗模式

    CLK_ACON0 |= CLK_AIP_HRC_EN(0x1); // 使能HRC时钟
    LP_WKPND |= LP_WKUP_0_PCLR(0x1);  // 清除通道0唤醒标志位
    CLK_CON0 |= CLK_SYSCLK_SEL(0x3);  // 系统时钟选择hirc_clk
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
    }

    P2_PND = p2_pnd; // 清P2中断标志位

    // -------------------------------------------------

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(P2_IRQn);
}

#endif // USE_STOP_MODE

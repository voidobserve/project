#include "stop_mode.h"

#if USE_STOP_MODE // ʹ��stopֹͣģʽ

void stop_mode_config(void)
{
    // P17����¼���ţ�û����IO_MAP�ر���¼�����������Ҫ������������
    P1_PU |= GPIO_P17_PULL_UP(0x1);

    // ����P25Ϊ��������
    P2_MD1 &= ~GPIO_P25_MODE_SEL(0x3); // ����P25Ϊ����ģʽ
    P2_PD |= GPIO_P25_PULL_PD(0x1);    // ����P25Ϊ����
    P2_IMK |= GPIO_P25_IRQ_MASK(0x1);  // ʹ��P25���ж�
    P2_TRG1 &= ~GPIO_P25_TRG_SEL(0x3); // ����P25�������ش���
    P2_TRG1 |= GPIO_P25_TRG_SEL(0x2);
    FIN_S10 = GPIO_FIN_SEL_P25; // ����P25Ϊͨ��0���뻽�Ѷ˿�

    __SetIRQnIP(P2_IRQn, P2_IQn_CFG); // �����ж����ȼ�
    __EnableIRQ(P2_IRQn);             // ʹ��P2�ж�
    IE_EA = 1;                        // ʹ���ܿ���

    // LP_WKPND = LP_WKUP_0_PCLR(0x1);  // ������ѱ�־λ
    // LP_WKCON = (LP_WKUP_0_EDG(0x0) | // ͨ��0�ߵ�ƽ��������
    //             LP_WKUP_0_EN(0x1));  // ����ͨ��0ʹ��

    CLK_CON0 &= ~CLK_SYSCLK_SEL(0x3);  // ϵͳʱ��ѡ��rc64k
    CLK_ACON0 &= ~CLK_AIP_HRC_EN(0x1); // �ر�HRCʱ��
    LP_CON |= LP_STOP_EN(0x1);         // ����Stop�͹���ģʽ

    CLK_ACON0 |= CLK_AIP_HRC_EN(0x1); // ʹ��HRCʱ��
    LP_WKPND |= LP_WKUP_0_PCLR(0x1);  // ���ͨ��0���ѱ�־λ
    CLK_CON0 |= CLK_SYSCLK_SEL(0x3);  // ϵͳʱ��ѡ��hirc_clk
}

// P2�жϷ�����
void P2_IRQHandler(void) interrupt P2_IRQn
{
    // P0_PND�Ĵ���д�κ�ֵ�������־λ
    u8 p2_pnd = P2_PND;

    // �����ж�����IP������ɾ��
    __IRQnIPnPush(P2_IRQn);

    // ---------------- �û��������� -------------------

    if (p2_pnd & GPIO_P25_IRQ_PNG(0x1))
    {
    }

    P2_PND = p2_pnd; // ��P2�жϱ�־λ

    // -------------------------------------------------

    // �˳��ж�����IP������ɾ��
    __IRQnIPnPop(P2_IRQn);
}

#endif // USE_STOP_MODE

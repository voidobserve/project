#include "idle_mode.h"
#if USE_IDLE_MODE

// ����ģʽ״̬��־λ����Ҫע����ǣ������ж϶��ܻ��Ѵ���ģʽ��������Ҫ��ÿ���ж����ñ�־λ�Ĵ���
volatile bit idle_mode_state = 0; 

#if 0 // ʹ��ʾ��

if (idle_mode_state == 0)
{
    LED6 = ~LED6;
    idle_mode_state = 1;
    idle_mode_config();
}

// ����־λΪ0ʱ�������ж�
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

// ����ģʽ����
// P25����Ϊ�������ţ������ػ���
void idle_mode_config(void)
{
#if MY_DEBUG
    printf("Idle in\n");
#endif

    // ���Բ������û���ͨ����ֻҪ���жϣ��Ϳ��Ի��Ѵ���ģʽ
    // LP_WKPND = LP_WKUP_0_PCLR(0x1);  // ������ѱ�־λ
    // LP_WKCON = (LP_WKUP_0_EDG(0x0) | // ͨ��0�ߵ�ƽ��������
    //             LP_WKUP_0_EN(0x1));  // ����ͨ��0ʹ��

    // �������ģʽ
    // �������ģʽ�󣬳����ͣ�����ֱ�������жϲ��˳�����ģʽ�������������
    LP_CON &= ~LP_IDLE_EN(0x1);
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
        idle_mode_state = 0;
    }

    P2_PND = p2_pnd; // ��P2�жϱ�־λ

    // -------------------------------------------------

    // �˳��ж�����IP������ɾ��
    __IRQnIPnPop(P2_IRQn);
}

#endif // USE_IDLE_MODE
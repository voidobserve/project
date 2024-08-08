#include "uart0.h"

#if USE_UART0

// #include <string.h> // memset()

// ���õĲ�������Ҫ���䵥Ƭ����ʱ�ӣ�����ֱ��ʹ���˹ٷ��Ĵ���
#define USER_UART0_BAUD ((SYSCLK - UART0_BAUDRATE) / (UART0_BAUDRATE))

// // ������Ž��յ�����֡�Ļ�����
// volatile u8 uart0_recv_buf[UART0_RXBUF_LEN] = {0};
// volatile u16 uart0_recv_cnt = 0;  // ���ջ���������ֵ
// volatile bit uart0_recv_flag = 0; // �������һ֡�ı�־λ

/*
    // ʹ��ʾ��
    if (uart0_recv_flag)
    {
        uart0_recv_flag = 0;
        printf("%s\n", uart0_recv_buf);
        memset(uart0_recv_buf, 0, UART0_RXBUF_LEN);
        uart0_recv_cnt = 0;
    }
*/

#if USE_PRINTF_UART0
// ��дputchar()����
char putchar(char c)
{
    uart0_sendbyte(c);
    return c;
}
#endif // USE_PRINTF_UART0

#ifdef DEVELOPMENT_BOARD
// uart0��ʼ��
// �������ɺ� UART0_BAUDRATE ������
void uart0_config(void)
{
    P2_MD1 &= (~GPIO_P25_MODE_SEL(0x3));
    P2_MD1 |= GPIO_P25_MODE_SEL(0x1); // ���ģʽ
    FOUT_S25 |= GPIO_FOUT_UART0_TX;   // ����P22ΪUART0_TX

    UART0_BAUD1 = (USER_UART0_BAUD >> 8) & 0xFF; // ���ò����ʸ߰�λ
    UART0_BAUD0 = USER_UART0_BAUD & 0xFF;        // ���ò����ʵͰ�λ
    UART0_CON0 = UART_STOP_BIT(0x0) |
                 UART_RX_IRQ_EN(0x1) |
                 UART_EN(0x1); // 8bit���ݣ�1bitֹͣλ��ʹ��RX�ж�
}
#endif // DEVELOPMENT_BOARD

#ifdef CIRCUIT_BOARD
// uart0��ʼ��
// �������ɺ� UART0_BAUDRATE ������
void uart0_init(void)
{
    // ��·���ϣ�P11Ϊ�������ţ�P12Ϊ��������
    P1_MD0 &= (~GPIO_P11_MODE_SEL(0x3) | ~GPIO_P12_MODE_SEL(0x3));
    P1_MD0 |= GPIO_P11_MODE_SEL(0x1); // ���ģʽ
    FOUT_S11 |= GPIO_FOUT_UART0_TX;   // ����P11ΪUART0_TX
    FIN_S7 |= GPIO_FIN_SEL_P12;       // ����P12ΪUART0_RX

    __EnableIRQ(UART0_IRQn); // ��UARTģ���ж�
    IE_EA = 1;               // �����ж�

    UART0_BAUD1 = (USER_UART0_BAUD >> 8) & 0xFF; // ���ò����ʸ߰�λ
    UART0_BAUD0 = USER_UART0_BAUD & 0xFF;        // ���ò����ʵͰ�λ
    UART0_CON0 = UART_STOP_BIT(0x0) |
                 UART_RX_IRQ_EN(0x1) |
                 UART_EN(0x1); // 8bit���ݣ�1bitֹͣλ��ʹ��RX�ж�

    // memset(uart0_recv_buf, 0, sizeof(uart0_recv_buf));
}
#endif // DEVELOPMENT_BOARD

// UART0����һ���ֽ����ݵĺ���
void uart0_sendbyte(u8 senddata)
{
    while (!(UART0_STA & UART_TX_DONE(0x01)))
        ;
    UART0_DATA = senddata;
    while (!(UART0_STA & UART_TX_DONE(0x01))) // �ȴ���η������
        ;
}

// // UART0�жϷ������������жϣ�
// void UART0_IRQHandler(void) interrupt UART0_IRQn
// {
//     // �����ж�����IP������ɾ��
//     __IRQnIPnPush(UART0_IRQn);
//     // ---------------- �û��������� -------------------
//     // RX��������ж�
//     if (UART0_STA & UART_RX_DONE(0x1))
//     {
//         uart0_recv_buf[uart0_recv_cnt++] = UART0_DATA;
//         if (UART0_DATA == '\n')
//         {
//             uart0_recv_flag = 1;
//         }
//     }

//     // �˳��ж�����IP������ɾ��
//     __IRQnIPnPop(UART0_IRQn);
// }

#endif // USE_UART0

#ifndef __IDLE_MODE_H
#define __IDLE_MODE_H

#include "include.h"   // ʹ��оƬ�ٷ��ṩ��ͷ�ļ�
#include "my_config.h" // �����Զ����ͷ�ļ�
#if USE_IDLE_MODE // ʹ��Idle����ģʽ

extern volatile bit idle_mode_state;

// ����ģʽ����
// P25����Ϊ�������ţ������ػ���
void idle_mode_config(void);

#endif // USE_IDLE_MODE 
#endif // __IDLE_MODE_H
#ifndef __GIT_H
#define __GIT_H
#include "stdio.h"
#include "stdlib.h"
#include <string.h>
#include <stdlib.h>

typedef unsigned char U8;
typedef signed char S8;
typedef unsigned short U16;
typedef signed short S16;
typedef unsigned int U32;
typedef signed int S32;
typedef float F32;

#define PROID "634589" // ��ƷID

#define AUTH_INFO "3" // ��Ȩ��Ϣ

#define DEVID "1215487236" // �豸ID

// �Զ���ִ�н������
typedef enum
{
	MY_SUCCESSFUL = 0x01, // �ɹ�
	MY_FAIL = 0x00		  // ʧ��

} mySta; // �ɹ���־λ

typedef enum
{
	OPEN = 0x01, // ��
	CLOSE = 0x00 // �ر�

} On_or_Off_TypeDef; // �ɹ���־λ

typedef enum
{
	DERVICE_SEND = 0x00, // �豸->ƽ̨
	PLATFORM_SEND = 0x01 // ƽ̨->�豸

} Send_directino; // ���ͷ���

typedef struct
{
	F32 muddy;		 // ���Ƕ�
	F32 temperatuer; // DS18B20�¶�
	F32 ph_value;	 // PH����

} Data_TypeDef; // ���ݲ����ṹ��

typedef struct
{

	F32 threshold_temperatuer_max; // �¶�
	F32 threshold_temperatuer_min; // �¶�

} Threshold_Value_TypeDef; // ���ݲ����ṹ��

typedef struct
{

	U8 Box1_state; // ����״̬
	U8 Box2_state; // ����״̬
	U8 Box3_state; // ����״̬

	U8 Box1_state_open; // ����״̬
	U8 Box2_state_open; // ����״̬
	U8 Box3_state_open; // ����״̬

} Device_Satte_Typedef; // ״̬�����ṹ��

typedef struct
{

	U16 Box1_Pwd; // ����һ
	U16 Box2_Pwd; // �����
	U16 Box3_Pwd; // ������

} Pwd_Box_Typedef; // ����

// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data);
// ��ʼ��
mySta Reset_Threshole_Value(Pwd_Box_Typedef *box_pwd, Device_Satte_Typedef *device_state);
// ����OLED��ʾ��������
mySta Update_oled_massage(void);
// �����豸״̬
mySta Update_device_massage(Device_Satte_Typedef *device_state);

void SG90_Turn(F32 turn);
// ��֤���벢����
void Input_Password(void);
// �Զ��رն��
void Automation_Close(void);
#endif

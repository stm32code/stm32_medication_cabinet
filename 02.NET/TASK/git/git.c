#include "git.h"
#include "delay.h"
#include "onenet.h"

// ����豸�����ļ�
#include "oled.h"
#include "pwm.h"
#include "beep.h"
#include "TTP229.h"

Data_TypeDef Data_init;						  // �豸���ݽṹ��
Threshold_Value_TypeDef threshold_value_init; // �豸��ֵ���ýṹ��
Device_Satte_Typedef device_state_init;		  // �豸״̬
Pwd_Box_Typedef box_pwd_init;				  // �豸����

extern char *Tips;
extern const char *topics[];
U8 key_num = 0;
U8 key_cope[4] = {0};
U8 Page = 0;
U8 Close_Box_Time, Error_Time;
U16 pwd = 0;

// ��ȡ���ݲ���
mySta Read_Data(Data_TypeDef *Device_Data)
{
	//	Device_Data->temperatuer = DS18B20_GetTemp_MatchRom(ucDs18b20Id); // ��ȡ�¶�
	// Device_Data->muddy = 11.5;
	//	Device_Data->muddy = TDS_Value_Conversion(CHECK1);
	//	Device_Data->ph_value = Get_Adc_Average(CHECK2, 2) - 3;

	return MY_SUCCESSFUL;
}
// ��ʼ��
mySta Reset_Threshole_Value(Pwd_Box_Typedef *box_pwd, Device_Satte_Typedef *device_state)
{
	// �����ʼ
	box_pwd->Box1_Pwd = 1111;
	box_pwd->Box2_Pwd = 2222;
	box_pwd->Box3_Pwd = 3333;
	// ״̬����
	device_state->Box1_state = 0;
	device_state->Box2_state = 0;
	device_state->Box3_state = 0;
	return MY_SUCCESSFUL;
}

// ����OLED��ʾ��������
mySta Update_oled_massage()
{
	char str[50];
	u8 i;
	if (Page == 0) // �����������
	{
		for (i = 0; i < 5; i++)
		{
			OLED_ShowChinese((18 * i + 20), 0, i, 16); // ��
		}
		for (i = 0; i < 4; i++)
		{
			sprintf(str, "%d", key_cope[i]);
			OLED_ShowString((18 * i + 35), 32, (unsigned char *)str, 16);
		}

		if (device_state_init.Box1_state_open == 1)
		{
			device_state_init.Box1_state = 0;
			device_state_init.Box1_state_open = 0;
		}
		if (device_state_init.Box2_state_open == 1)
		{
			device_state_init.Box2_state = 0;
			device_state_init.Box2_state_open = 0;
		}
		if (device_state_init.Box3_state_open == 1)
		{
			device_state_init.Box3_state = 0;
			device_state_init.Box3_state_open = 0;
		}
	}
	else if (Page == 1) // �������
	{

		for (i = 0; i < 4; i++)
		{
			OLED_ShowChinese((18 * i + 10), 0, i + 5, 16); // ��
		}
		for (i = 0; i < 4; i++)
		{
			sprintf(str, ": %02d", Close_Box_Time);
			OLED_ShowString(86, 0, (unsigned char *)str, 16);
		}
		if (Close_Box_Time == 0)
		{

			Page = 0; // ���ؽ���һ
			memset(key_cope, 0, 4);
			key_num = 0;
			pwd = 0;
			OLED_Clear();
		}
	}
	else if (Page == 2) // ����������
	{

		for (i = 0; i < 4; i++)
		{
			OLED_ShowChinese((18 * i + 10), 0, i + 10, 16); //
		}
		for (i = 0; i < 4; i++)
		{
			sprintf(str, ": %02d", Error_Time);
			OLED_ShowString(86, 0, (unsigned char *)str, 16);
		}
		if (Error_Time == 0)
		{
			Page = 0; // ���ؽ���һ
			memset(key_cope, 0, 4);
			key_num = 0;
			pwd = 0;
			OLED_Clear();
		}
	}

	OLED_Refresh(); // ������ʾ

	return MY_SUCCESSFUL;
}

// ��֤���벢����
void Input_Password()
{
	char str[50];
	U8 key = 0;
	key = Touch_KeyScan();
	if (key != 0)
	{
		if (key == 10 && key_num > 0) // A (ɾ��)
		{
			key_num--;
			key_cope[key_num] = 0;
		}
		else if (key == 13) // *  ������һ��
		{
			if (Page != 0) // ������һ��
			{
				Page = 0;
			}
			pwd = 0;
			memset(key_cope, 0, 4);
			OLED_Clear();
			key_num = 0;
		}
		else if (key == 16) // D ���
		{
			memset(key_cope, 0, 4);
			key_num = 0;
		}
		else if (key == 15) // # ȷ��
		{
			// printf("pwd_copy:%d\r\n", pwd);
			if (pwd == box_pwd_init.Box1_Pwd)
			{
				while (OneNet_Subscribe(topics, 1)) // ����OneNET
					delay_ms(10);
				sprintf(str, "BOX:1:%d", pwd);
				while (OneNet_Publish(Tips, str))
					delay_ms(10);
				Page = 1;
				OLED_Clear();
				Close_Box_Time = 10;
				device_state_init.Box1_state_open = 1;
			}
			else if (pwd == box_pwd_init.Box2_Pwd)
			{
				while (OneNet_Subscribe(topics, 1)) // ����OneNET
					delay_ms(10);
				sprintf(str, "BOX:2:%d", pwd);
				while (OneNet_Publish(Tips, str))
					delay_ms(10);
				Page = 1;
				OLED_Clear();
				Close_Box_Time = 10;
				device_state_init.Box2_state_open = 1;
			}
			else if (pwd == box_pwd_init.Box3_Pwd)
			{
				while (OneNet_Subscribe(topics, 1)) // ����OneNET
					delay_ms(10);
				sprintf(str, "BOX:3:%d", pwd);
				while (OneNet_Publish(Tips, str))
					delay_ms(10);
				Page = 1;
				OLED_Clear();
				Close_Box_Time = 10;
				device_state_init.Box3_state_open = 1;
			}
			else // �������
			{
				Page = 2;
				OLED_Clear();
				Error_Time = 3;
			}
		}
		else if (key == 14 && key_num > 0) // 0 ����
		{

			key_cope[key_num++] = 0;
		}
		else if (key_num < 4 && key != 10 && key != 16 && key != 15 && key != 11 && key != 12 && key != 13)
		{
			switch (key_num)
			{
			case 0:
				pwd = key * 1000 + pwd;
				break;
			case 1:
				pwd = key * 100 + pwd;
				break;
			case 2:
				pwd = key * 10 + pwd;
				break;
			case 3:
				pwd = key + pwd;
				break;
			default:
				break;
			}
			key_cope[key_num++] = key;
		}
		BEEP_ON_Time(25);
		printf("key:%02d,%d\r\n", key, key_num);
	}
}
// ת��
void SG90_Turn(F32 turn)
{

	if (turn < 30)
	{
		turn = turn + 6;
	}

	Specific_Value(TIMX_2, 2, turn / 36);
	Specific_Value(TIMX_2, 3, turn / 36);
	Specific_Value(TIMX_2, 4, turn / 36);
}
// �����豸״̬
mySta Update_device_massage(Device_Satte_Typedef *device_state)
{
	// ���в���ִֻ��һ�Σ���Լϵͳ��Դ,

	// if (!1) // �Ƿ���APP����
	// {

	// }

	return MY_SUCCESSFUL;
}
// �Զ��رն��
void Automation_Close(void)
{
	if (device_state_init.Box1_state_open == 1)
	{
		Specific_Value(TIMX_2, 2, 3.5); // ���ƶ��
	}
	else if (device_state_init.Box1_state_open == 0)
	{
		Specific_Value(TIMX_2, 2, 1); // ���ƶ��
	}
	if (device_state_init.Box2_state_open == 1)
	{
		Specific_Value(TIMX_2, 3, 3.5); // ���ƶ��
	}
	else if (device_state_init.Box2_state_open == 0)
	{
		Specific_Value(TIMX_2, 3, 1); // ���ƶ��
	}
	if (device_state_init.Box3_state_open == 1)
	{
		Specific_Value(TIMX_2, 4, 3.5); // ���ƶ��
	}
	else if (device_state_init.Box3_state_open == 0)
	{
		Specific_Value(TIMX_2, 4, 1); // ���ƶ��
	}
}

#include "git.h"
#include "delay.h"
#include "onenet.h"

// 添加设备配置文件
#include "oled.h"
#include "pwm.h"
#include "beep.h"
#include "TTP229.h"

Data_TypeDef Data_init;						  // 设备数据结构体
Threshold_Value_TypeDef threshold_value_init; // 设备阈值设置结构体
Device_Satte_Typedef device_state_init;		  // 设备状态
Pwd_Box_Typedef box_pwd_init;				  // 设备密码

extern char *Tips;
extern const char *topics[];
U8 key_num = 0;
U8 key_cope[4] = {0};
U8 Page = 0;
U8 Close_Box_Time, Error_Time;
U16 pwd = 0;

// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data)
{
	//	Device_Data->temperatuer = DS18B20_GetTemp_MatchRom(ucDs18b20Id); // 获取温度
	// Device_Data->muddy = 11.5;
	//	Device_Data->muddy = TDS_Value_Conversion(CHECK1);
	//	Device_Data->ph_value = Get_Adc_Average(CHECK2, 2) - 3;

	return MY_SUCCESSFUL;
}
// 初始化
mySta Reset_Threshole_Value(Pwd_Box_Typedef *box_pwd, Device_Satte_Typedef *device_state)
{
	// 密码初始
	box_pwd->Box1_Pwd = 1111;
	box_pwd->Box2_Pwd = 2222;
	box_pwd->Box3_Pwd = 3333;
	// 状态重置
	device_state->Box1_state = 0;
	device_state->Box2_state = 0;
	device_state->Box3_state = 0;
	return MY_SUCCESSFUL;
}

// 更新OLED显示屏中内容
mySta Update_oled_massage()
{
	char str[50];
	u8 i;
	if (Page == 0) // 输入密码界面
	{
		for (i = 0; i < 5; i++)
		{
			OLED_ShowChinese((18 * i + 20), 0, i, 16); // 中
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
	else if (Page == 1) // 开箱界面
	{

		for (i = 0; i < 4; i++)
		{
			OLED_ShowChinese((18 * i + 10), 0, i + 5, 16); // 中
		}
		for (i = 0; i < 4; i++)
		{
			sprintf(str, ": %02d", Close_Box_Time);
			OLED_ShowString(86, 0, (unsigned char *)str, 16);
		}
		if (Close_Box_Time == 0)
		{

			Page = 0; // 返回界面一
			memset(key_cope, 0, 4);
			key_num = 0;
			pwd = 0;
			OLED_Clear();
		}
	}
	else if (Page == 2) // 密码错误界面
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
			Page = 0; // 返回界面一
			memset(key_cope, 0, 4);
			key_num = 0;
			pwd = 0;
			OLED_Clear();
		}
	}

	OLED_Refresh(); // 开启显示

	return MY_SUCCESSFUL;
}

// 验证密码并开锁
void Input_Password()
{
	char str[50];
	U8 key = 0;
	key = Touch_KeyScan();
	if (key != 0)
	{
		if (key == 10 && key_num > 0) // A (删除)
		{
			key_num--;
			key_cope[key_num] = 0;
		}
		else if (key == 13) // *  返回上一级
		{
			if (Page != 0) // 返回上一级
			{
				Page = 0;
			}
			pwd = 0;
			memset(key_cope, 0, 4);
			OLED_Clear();
			key_num = 0;
		}
		else if (key == 16) // D 清空
		{
			memset(key_cope, 0, 4);
			key_num = 0;
		}
		else if (key == 15) // # 确认
		{
			// printf("pwd_copy:%d\r\n", pwd);
			if (pwd == box_pwd_init.Box1_Pwd)
			{
				while (OneNet_Subscribe(topics, 1)) // 接入OneNET
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
				while (OneNet_Subscribe(topics, 1)) // 接入OneNET
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
				while (OneNet_Subscribe(topics, 1)) // 接入OneNET
					delay_ms(10);
				sprintf(str, "BOX:3:%d", pwd);
				while (OneNet_Publish(Tips, str))
					delay_ms(10);
				Page = 1;
				OLED_Clear();
				Close_Box_Time = 10;
				device_state_init.Box3_state_open = 1;
			}
			else // 密码错误
			{
				Page = 2;
				OLED_Clear();
				Error_Time = 3;
			}
		}
		else if (key == 14 && key_num > 0) // 0 特殊
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
// 转向
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
// 更新设备状态
mySta Update_device_massage(Device_Satte_Typedef *device_state)
{
	// 所有操作只执行一次，节约系统资源,

	// if (!1) // 是否是APP控制
	// {

	// }

	return MY_SUCCESSFUL;
}
// 自动关闭舵机
void Automation_Close(void)
{
	if (device_state_init.Box1_state_open == 1)
	{
		Specific_Value(TIMX_2, 2, 3.5); // 控制舵机
	}
	else if (device_state_init.Box1_state_open == 0)
	{
		Specific_Value(TIMX_2, 2, 1); // 控制舵机
	}
	if (device_state_init.Box2_state_open == 1)
	{
		Specific_Value(TIMX_2, 3, 3.5); // 控制舵机
	}
	else if (device_state_init.Box2_state_open == 0)
	{
		Specific_Value(TIMX_2, 3, 1); // 控制舵机
	}
	if (device_state_init.Box3_state_open == 1)
	{
		Specific_Value(TIMX_2, 4, 3.5); // 控制舵机
	}
	else if (device_state_init.Box3_state_open == 0)
	{
		Specific_Value(TIMX_2, 4, 1); // 控制舵机
	}
}

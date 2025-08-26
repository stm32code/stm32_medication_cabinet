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

#define PROID "634589" // 产品ID

#define AUTH_INFO "3" // 鉴权信息

#define DEVID "1215487236" // 设备ID

// 自定义执行结果类型
typedef enum
{
	MY_SUCCESSFUL = 0x01, // 成功
	MY_FAIL = 0x00		  // 失败

} mySta; // 成功标志位

typedef enum
{
	OPEN = 0x01, // 打开
	CLOSE = 0x00 // 关闭

} On_or_Off_TypeDef; // 成功标志位

typedef enum
{
	DERVICE_SEND = 0x00, // 设备->平台
	PLATFORM_SEND = 0x01 // 平台->设备

} Send_directino; // 发送方向

typedef struct
{
	F32 muddy;		 // 浑浊度
	F32 temperatuer; // DS18B20温度
	F32 ph_value;	 // PH数据

} Data_TypeDef; // 数据参数结构体

typedef struct
{

	F32 threshold_temperatuer_max; // 温度
	F32 threshold_temperatuer_min; // 温度

} Threshold_Value_TypeDef; // 数据参数结构体

typedef struct
{

	U8 Box1_state; // 箱体状态
	U8 Box2_state; // 箱体状态
	U8 Box3_state; // 箱体状态

	U8 Box1_state_open; // 箱体状态
	U8 Box2_state_open; // 箱体状态
	U8 Box3_state_open; // 箱体状态

} Device_Satte_Typedef; // 状态参数结构体

typedef struct
{

	U16 Box1_Pwd; // 密码一
	U16 Box2_Pwd; // 密码二
	U16 Box3_Pwd; // 密码三

} Pwd_Box_Typedef; // 密码

// 获取数据参数
mySta Read_Data(Data_TypeDef *Device_Data);
// 初始化
mySta Reset_Threshole_Value(Pwd_Box_Typedef *box_pwd, Device_Satte_Typedef *device_state);
// 更新OLED显示屏中内容
mySta Update_oled_massage(void);
// 更新设备状态
mySta Update_device_massage(Device_Satte_Typedef *device_state);

void SG90_Turn(F32 turn);
// 验证密码并开锁
void Input_Password(void);
// 自动关闭舵机
void Automation_Close(void);
#endif

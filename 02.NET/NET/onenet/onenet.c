// 网络设备
#include "esp8266.h"

// 协议文件
#include "onenet.h"
#include "mqttkit.h"
#include "delay.h"

// 硬件驱动
#include "usart.h"
#include "oled.h"
// C库
#include <string.h>
#include <stdio.h>

extern Data_TypeDef Data_init;
extern Device_Satte_Typedef device_state_init; // 设备状态
extern Pwd_Box_Typedef box_pwd_init;		   // 设备密码
extern unsigned char esp8266_buf[128];
extern char *Tips;
extern const char *topics[];
extern U8 Page;
extern U8 Close_Box_Time;
U8 Connect_Net = 0;
//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包

	unsigned char *dataPtr;

	_Bool status = 1;

	printf("OneNet_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);
	// num++;
	// if (num > 1)
	// {
	// 	Sys_Restart(); // 软件复位
	// 	num = 0;
	// }
	if (MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len); // 上传平台

		dataPtr = ESP8266_GetIPD(250); // 等待平台响应
		if (dataPtr != NULL)
		{
			if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch (MQTT_UnPacketConnectAck(dataPtr))
				{
				case 0:
					printf("Tips:	连接成功\r\n");
					status = 0;
					break;

				case 1:
					printf("WARN:	连接失败：协议错误\r\n");
					break;
				case 2:
					printf("WARN:	连接失败：非法的clientid\r\n");
					break;
				case 3:
					printf("WARN:	连接失败：服务器失败\r\n");
					break;
				case 4:
					printf("WARN:	连接失败：用户名或密码错误\r\n");
					break;
				case 5:
					printf("WARN:	连接失败：非法链接(比如token非法)\r\n");
					break;

				default:
					printf("ERR:	连接失败：未知错误\r\n");
					break;
				}
			}
		}

		MQTT_DeleteBuffer(&mqttPacket); // 删包
	}
	else
		printf("WARN:	MQTT_PacketConnect Failed\r\n");

	return status;
}
unsigned char OneNet_FillBuf(char *buf)
{
	char text[100];

	memset(text, 0, sizeof(text));

	strcpy(buf, ",;");

	memset(text, 0, sizeof(text));
	sprintf(text, "Water,%d;", 0); // Temp是数据流的一个名称，temper是温度值
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "Temp,%.2f;", Data_init.temperatuer); // Temp是数据流的一个名称，temper是温度值
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "Muddy,%.1f;", Data_init.muddy); // Temp是数据流的一个名称，temper是温度值
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "PH,%.1f;", Data_init.ph_value); // Temp是数据流的一个名称，temper是温度值
	strcat(buf, text);

	return strlen(buf);
}
unsigned char MQTT_FillBuf(char *buf)
{
	char text[126];
	memset(text, 0, sizeof(text));

	strcpy(buf, "{");

	memset(text, 0, sizeof(text));
	sprintf(text, "\"box1\":\"%d\",", device_state_init.Box1_state); // Temp是数据流的一个名称，temper是温度值
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"box2\":\"%d\",", device_state_init.Box2_state); // Temp是数据流的一个名称，temper是温度值
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"box3\":\"%d\"", device_state_init.Box3_state); // Temp是数据流的一个名称，temper是温度值
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "}");
	strcat(buf, text);

	return strlen(buf);
}
//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：
//==========================================================
void OneNet_SendData(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包

	char buf[128];

	short body_len = 0, i = 0;

	//	printf("Tips:	OneNet_SendData-MQTT\r\n");

	memset(buf, 0, sizeof(buf));

	body_len = OneNet_FillBuf(buf); // 获取当前需要发送的数据流的总长度

	if (body_len)
	{
		if (MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0) // 封包
		{
			for (; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];

			ESP8266_SendData(mqttPacket._data, mqttPacket._len); // 上传数据到平台
			//			printf("Send %d Bytes\r\n", mqttPacket._len);

			MQTT_DeleteBuffer(&mqttPacket); // 删包
		}
		else
			printf("WARN:	EDP_NewBuffer Failed\r\n");
	}
}
//==========================================================
//	函数名称：	OneNet_SendData
//
//	函数功能：	上传数据到平台
//
//	入口参数：	type：发送数据的格式
//
//	返回参数：	无
//
//	说明：
//==========================================================
void OneNet_SendMqtt(void)
{

	char buf[128];

	OneNet_Subscribe(topics, 1);
	MQTT_FillBuf(buf);
	while (OneNet_Publish(Tips, buf))
		delay_ms(10);
}
//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包

	char *req_payload = NULL;
	char *cmdid_topic = NULL;

	unsigned short topic_len = 0;
	unsigned short req_len = 0;

	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;

	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	type = MQTT_UnPacketRecv(cmd);
	Connect_Net = 0;
	switch (type)
	{

	case MQTT_PKT_CMD: // 命令下发

		result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len); // 解出topic和消息体
		if (result == 0)
		{
			printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

			if (MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0) // 命令回复组包
			{
				printf("Tips:	Send CmdResp\r\n");

				ESP8266_SendData(mqttPacket._data, mqttPacket._len); // 回复命令
				MQTT_DeleteBuffer(&mqttPacket);						 // 删包
			}
		}

		break;
	case MQTT_PKT_PUBLISH: // 接收的Publish消息

		result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
		if (result == 0)
		{
			printf("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
				   cmdid_topic, topic_len, req_payload, req_len);

			switch (qos)
			{
			case 1: // 收到publish的qos为1，设备需要回复Ack

				if (MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
				{
					// printf( "Tips:	Send PublishAck\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}

				break;

			case 2: // 收到publish的qos为2，设备先回复Rec
					// 平台回复Rel，设备再回复Comp
				if (MQTT_PacketPublishRec(pkt_id, &mqttPacket) == 0)
				{
					// printf( "Tips:	Send PublishRec\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}

				break;

			default:
				break;
			}
		}

		break;

	case MQTT_PKT_PUBACK: // 发送Publish消息，平台回复的Ack

		if (MQTT_UnPacketPublishAck(cmd) == 0)
			// printf( "Tips:	MQTT Publish Send OK\r\n");

			break;

	case MQTT_PKT_PUBREC: // 发送Publish消息，平台回复的Rec，设备需回复Rel消息

		if (MQTT_UnPacketPublishRec(cmd) == 0)
		{
			// printf( "Tips:	Rev PublishRec\r\n");
			if (MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
			{
				// printf( "Tips:	Send PublishRel\r\n");
				ESP8266_SendData(mqttPacket._data, mqttPacket._len);
				MQTT_DeleteBuffer(&mqttPacket);
			}
		}

		break;

	case MQTT_PKT_PUBREL: // 收到Publish消息，设备回复Rec后，平台回复的Rel，设备需再回复Comp

		if (MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
		{
			// printf( "Tips:	Rev PublishRel\r\n");
			if (MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
			{
				// printf( "Tips:	Send PublishComp\r\n");
				ESP8266_SendData(mqttPacket._data, mqttPacket._len);
				MQTT_DeleteBuffer(&mqttPacket);
			}
		}

		break;

	case MQTT_PKT_PUBCOMP: // 发送Publish消息，平台返回Rec，设备回复Rel，平台再返回的Comp

		if (MQTT_UnPacketPublishComp(cmd) == 0)
		{
			// LinkWifi_Falge=1;
			// printf( "Tips:	Rev PublishComp\r\n");
		}

		break;

	case MQTT_PKT_SUBACK: // 发送Subscribe消息的Ack

		if (MQTT_UnPacketSubscribe(cmd) == 0)
			printf("Tips:	MQTT Subscribe OK\r\n");
		else
			printf("Tips:	MQTT Subscribe Err\r\n");

		break;

	case MQTT_PKT_UNSUBACK: // 发送UnSubscribe消息的Ack

		if (MQTT_UnPacketUnSubscribe(cmd) == 0)
			printf("Tips:	MQTT UnSubscribe OK\r\n");
		else
			printf("Tips:	MQTT UnSubscribe Err\r\n");

		break;

	default:
		result = -1;
		break;
	}

	ESP8266_Clear(); // 清空缓存

	if (result == -1)
		return;

	dataPtr = strchr(req_payload, ':'); // 搜索':'

	if (dataPtr != NULL && result != -1) // 如果找到了
	{

		dataPtr++;
		while (*dataPtr >= '0' && *dataPtr <= '9') // 判断是否是下发的命令控制数据
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;

		num = atoi((const char *)numBuf); // 转为数值形式
		// printf("num:%d\r\n", num);

		if (strstr((char *)req_payload, "PWD")) // 搜索"redled"
		{
			char charrTemp[4] = {0};
			// printf("req_payload:%s\r\n", req_payload);
			strncpy(charrTemp, req_payload + 6, 4);
			printf("req_payload:%s\r\n", charrTemp);
			switch (num)
			{
			case 1:
				box_pwd_init.Box1_Pwd = atoi(charrTemp) / 10;
				device_state_init.Box1_state = 1;
				printf("Box1_Pwd:%d\r\n", box_pwd_init.Box1_Pwd);
				break;
			case 2:
				box_pwd_init.Box2_Pwd = atoi(charrTemp) / 10;
				device_state_init.Box2_state = 1;
				printf("Box2_Pwd:%d\r\n", box_pwd_init.Box2_Pwd);
				break;
			case 3:
				box_pwd_init.Box3_Pwd = atoi(charrTemp) / 10;
				device_state_init.Box3_state = 1;
				printf("Box3_Pwd:%d\r\n", box_pwd_init.Box3_Pwd);
				break;
			default:
				break;
			}
		}
		if (strstr((char *)req_payload, "GETBOX"))
		{
			OneNet_SendMqtt();
			delay_ms(5); // 发送数据到APP
			ESP8266_Clear();
		}
		if (strstr((char *)req_payload, "BOX"))
		{
			switch (num)
			{
			case 1:
				while (OneNet_Subscribe(topics, 1)) // 接入OneNET
					delay_ms(10);
				while (OneNet_Publish(Tips, "BOX:1:ok"))
					delay_ms(10);
				if (device_state_init.Box1_state == 1)
				{
					Page = 1;
					OLED_Clear();
					Close_Box_Time = 10;
					device_state_init.Box1_state = 0;
					device_state_init.Box1_state_open = 1;
				}
				break;
			case 2:
				while (OneNet_Subscribe(topics, 1)) // 接入OneNET
					delay_ms(10);
				while (OneNet_Publish(Tips, "BOX:2:ok"))
					delay_ms(10);
				if (device_state_init.Box2_state == 1)
				{
					Page = 1;
					OLED_Clear();
					Close_Box_Time = 10;
					device_state_init.Box2_state = 0;
					device_state_init.Box2_state_open = 1;
				}
				break;
			case 3:
				while (OneNet_Subscribe(topics, 1)) // 接入OneNET
					delay_ms(10);
				while (OneNet_Publish(Tips, "BOX:3:ok"))
					delay_ms(10);
				if (device_state_init.Box3_state == 1)
				{
					Page = 1;
					OLED_Clear();
					Close_Box_Time = 10;
					device_state_init.Box3_state = 0;
					device_state_init.Box3_state_open = 1;
				}
				break;
			default:
				break;
			}
			delay_ms(5); // 发送数据到APP
			ESP8266_Clear();
		}
	}

	if (type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}
}

//==========================================================
//	函数名称：	void Link_OneNet(u8 Link)
//
//	函数功能：	连接判断
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：
//==========================================================
void Link_OneNet(u8 Link)
{

	if (!Link)
	{
		ESP8266_Init();			 // 初始化ESP8266
		while (OneNet_DevLink()) // 接入OneNET
			delay_ms(500);
	}
}
//==========================================================
//	函数名称：	OneNet_Subscribe
//
//	函数功能：	订阅
//
//	入口参数：	topics：订阅的topic
//				topic_cnt：topic个数
//
//	返回参数：	SEND_TYPE_OK-成功	SEND_TYPE_SUBSCRIBE-需要重发
//
//	说明：
//==========================================================
_Bool OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{

	unsigned char i = 0;

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包

	for (; i < topic_cnt; i++)
	{
		if (MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
		{
			ESP8266_SendData(mqttPacket._data, mqttPacket._len); // 向平台发送订阅请求

			MQTT_DeleteBuffer(&mqttPacket); // 删包
		}
		delay_ms(100);
	}

	return 0;
}
//==========================================================
//	函数名称：	OneNet_Publish
//
//	函数功能：	发布消息
//
//	入口参数：	topic：发布的主题
//				msg：消息内容
//
//	返回参数：	0-成功	1-需要重送
//
//	说明：
//==========================================================
_Bool OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // 协议包

	// printf( "Publish Topic: %s, Msg: %s\r\n", topic, msg);

	if (MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL2, 0, 1, &mqttPacket) != 1)
	{

		ESP8266_SendData(mqttPacket._data, mqttPacket._len); // 向平台发送订阅请求
		MQTT_DeleteBuffer(&mqttPacket);						 // 删包
	}
	return 0;
}

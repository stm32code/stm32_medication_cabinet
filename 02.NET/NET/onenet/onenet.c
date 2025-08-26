// �����豸
#include "esp8266.h"

// Э���ļ�
#include "onenet.h"
#include "mqttkit.h"
#include "delay.h"

// Ӳ������
#include "usart.h"
#include "oled.h"
// C��
#include <string.h>
#include <stdio.h>

extern Data_TypeDef Data_init;
extern Device_Satte_Typedef device_state_init; // �豸״̬
extern Pwd_Box_Typedef box_pwd_init;		   // �豸����
extern unsigned char esp8266_buf[128];
extern char *Tips;
extern const char *topics[];
extern U8 Page;
extern U8 Close_Box_Time;
U8 Connect_Net = 0;
//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

	unsigned char *dataPtr;

	_Bool status = 1;

	printf("OneNet_DevLink\r\nPROID: %s,	AUIF: %s,	DEVID:%s\r\n", PROID, AUTH_INFO, DEVID);
	// num++;
	// if (num > 1)
	// {
	// 	Sys_Restart(); // �����λ
	// 	num = 0;
	// }
	if (MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len); // �ϴ�ƽ̨

		dataPtr = ESP8266_GetIPD(250); // �ȴ�ƽ̨��Ӧ
		if (dataPtr != NULL)
		{
			if (MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch (MQTT_UnPacketConnectAck(dataPtr))
				{
				case 0:
					printf("Tips:	���ӳɹ�\r\n");
					status = 0;
					break;

				case 1:
					printf("WARN:	����ʧ�ܣ�Э�����\r\n");
					break;
				case 2:
					printf("WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");
					break;
				case 3:
					printf("WARN:	����ʧ�ܣ�������ʧ��\r\n");
					break;
				case 4:
					printf("WARN:	����ʧ�ܣ��û������������\r\n");
					break;
				case 5:
					printf("WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");
					break;

				default:
					printf("ERR:	����ʧ�ܣ�δ֪����\r\n");
					break;
				}
			}
		}

		MQTT_DeleteBuffer(&mqttPacket); // ɾ��
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
	sprintf(text, "Water,%d;", 0); // Temp����������һ�����ƣ�temper���¶�ֵ
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "Temp,%.2f;", Data_init.temperatuer); // Temp����������һ�����ƣ�temper���¶�ֵ
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "Muddy,%.1f;", Data_init.muddy); // Temp����������һ�����ƣ�temper���¶�ֵ
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "PH,%.1f;", Data_init.ph_value); // Temp����������һ�����ƣ�temper���¶�ֵ
	strcat(buf, text);

	return strlen(buf);
}
unsigned char MQTT_FillBuf(char *buf)
{
	char text[126];
	memset(text, 0, sizeof(text));

	strcpy(buf, "{");

	memset(text, 0, sizeof(text));
	sprintf(text, "\"box1\":\"%d\",", device_state_init.Box1_state); // Temp����������һ�����ƣ�temper���¶�ֵ
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"box2\":\"%d\",", device_state_init.Box2_state); // Temp����������һ�����ƣ�temper���¶�ֵ
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "\"box3\":\"%d\"", device_state_init.Box3_state); // Temp����������һ�����ƣ�temper���¶�ֵ
	strcat(buf, text);

	memset(text, 0, sizeof(text));
	sprintf(text, "}");
	strcat(buf, text);

	return strlen(buf);
}
//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����
//==========================================================
void OneNet_SendData(void)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

	char buf[128];

	short body_len = 0, i = 0;

	//	printf("Tips:	OneNet_SendData-MQTT\r\n");

	memset(buf, 0, sizeof(buf));

	body_len = OneNet_FillBuf(buf); // ��ȡ��ǰ��Ҫ���͵����������ܳ���

	if (body_len)
	{
		if (MQTT_PacketSaveData(DEVID, body_len, NULL, 5, &mqttPacket) == 0) // ���
		{
			for (; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];

			ESP8266_SendData(mqttPacket._data, mqttPacket._len); // �ϴ����ݵ�ƽ̨
			//			printf("Send %d Bytes\r\n", mqttPacket._len);

			MQTT_DeleteBuffer(&mqttPacket); // ɾ��
		}
		else
			printf("WARN:	EDP_NewBuffer Failed\r\n");
	}
}
//==========================================================
//	�������ƣ�	OneNet_SendData
//
//	�������ܣ�	�ϴ����ݵ�ƽ̨
//
//	��ڲ�����	type���������ݵĸ�ʽ
//
//	���ز�����	��
//
//	˵����
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
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

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

	case MQTT_PKT_CMD: // �����·�

		result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len); // ���topic����Ϣ��
		if (result == 0)
		{
			printf("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

			if (MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0) // ����ظ����
			{
				printf("Tips:	Send CmdResp\r\n");

				ESP8266_SendData(mqttPacket._data, mqttPacket._len); // �ظ�����
				MQTT_DeleteBuffer(&mqttPacket);						 // ɾ��
			}
		}

		break;
	case MQTT_PKT_PUBLISH: // ���յ�Publish��Ϣ

		result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
		if (result == 0)
		{
			printf("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
				   cmdid_topic, topic_len, req_payload, req_len);

			switch (qos)
			{
			case 1: // �յ�publish��qosΪ1���豸��Ҫ�ظ�Ack

				if (MQTT_PacketPublishAck(pkt_id, &mqttPacket) == 0)
				{
					// printf( "Tips:	Send PublishAck\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}

				break;

			case 2: // �յ�publish��qosΪ2���豸�Ȼظ�Rec
					// ƽ̨�ظ�Rel���豸�ٻظ�Comp
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

	case MQTT_PKT_PUBACK: // ����Publish��Ϣ��ƽ̨�ظ���Ack

		if (MQTT_UnPacketPublishAck(cmd) == 0)
			// printf( "Tips:	MQTT Publish Send OK\r\n");

			break;

	case MQTT_PKT_PUBREC: // ����Publish��Ϣ��ƽ̨�ظ���Rec���豸��ظ�Rel��Ϣ

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

	case MQTT_PKT_PUBREL: // �յ�Publish��Ϣ���豸�ظ�Rec��ƽ̨�ظ���Rel���豸���ٻظ�Comp

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

	case MQTT_PKT_PUBCOMP: // ����Publish��Ϣ��ƽ̨����Rec���豸�ظ�Rel��ƽ̨�ٷ��ص�Comp

		if (MQTT_UnPacketPublishComp(cmd) == 0)
		{
			// LinkWifi_Falge=1;
			// printf( "Tips:	Rev PublishComp\r\n");
		}

		break;

	case MQTT_PKT_SUBACK: // ����Subscribe��Ϣ��Ack

		if (MQTT_UnPacketSubscribe(cmd) == 0)
			printf("Tips:	MQTT Subscribe OK\r\n");
		else
			printf("Tips:	MQTT Subscribe Err\r\n");

		break;

	case MQTT_PKT_UNSUBACK: // ����UnSubscribe��Ϣ��Ack

		if (MQTT_UnPacketUnSubscribe(cmd) == 0)
			printf("Tips:	MQTT UnSubscribe OK\r\n");
		else
			printf("Tips:	MQTT UnSubscribe Err\r\n");

		break;

	default:
		result = -1;
		break;
	}

	ESP8266_Clear(); // ��ջ���

	if (result == -1)
		return;

	dataPtr = strchr(req_payload, ':'); // ����':'

	if (dataPtr != NULL && result != -1) // ����ҵ���
	{

		dataPtr++;
		while (*dataPtr >= '0' && *dataPtr <= '9') // �ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		numBuf[num] = 0;

		num = atoi((const char *)numBuf); // תΪ��ֵ��ʽ
		// printf("num:%d\r\n", num);

		if (strstr((char *)req_payload, "PWD")) // ����"redled"
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
			delay_ms(5); // �������ݵ�APP
			ESP8266_Clear();
		}
		if (strstr((char *)req_payload, "BOX"))
		{
			switch (num)
			{
			case 1:
				while (OneNet_Subscribe(topics, 1)) // ����OneNET
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
				while (OneNet_Subscribe(topics, 1)) // ����OneNET
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
				while (OneNet_Subscribe(topics, 1)) // ����OneNET
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
			delay_ms(5); // �������ݵ�APP
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
//	�������ƣ�	void Link_OneNet(u8 Link)
//
//	�������ܣ�	�����ж�
//
//	��ڲ�����	dataPtr��ƽ̨���ص�����
//
//	���ز�����	��
//
//	˵����
//==========================================================
void Link_OneNet(u8 Link)
{

	if (!Link)
	{
		ESP8266_Init();			 // ��ʼ��ESP8266
		while (OneNet_DevLink()) // ����OneNET
			delay_ms(500);
	}
}
//==========================================================
//	�������ƣ�	OneNet_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_SUBSCRIBE-��Ҫ�ط�
//
//	˵����
//==========================================================
_Bool OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{

	unsigned char i = 0;

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

	for (; i < topic_cnt; i++)
	{
		if (MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL2, topics, topic_cnt, &mqttPacket) == 0)
		{
			ESP8266_SendData(mqttPacket._data, mqttPacket._len); // ��ƽ̨���Ͷ�������

			MQTT_DeleteBuffer(&mqttPacket); // ɾ��
		}
		delay_ms(100);
	}

	return 0;
}
//==========================================================
//	�������ƣ�	OneNet_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	0-�ɹ�	1-��Ҫ����
//
//	˵����
//==========================================================
_Bool OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0}; // Э���

	// printf( "Publish Topic: %s, Msg: %s\r\n", topic, msg);

	if (MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL2, 0, 1, &mqttPacket) != 1)
	{

		ESP8266_SendData(mqttPacket._data, mqttPacket._len); // ��ƽ̨���Ͷ�������
		MQTT_DeleteBuffer(&mqttPacket);						 // ɾ��
	}
	return 0;
}

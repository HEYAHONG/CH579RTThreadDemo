#ifndef __MQTT_H__
#define __MQTT_H__
#include "rtconfig.h"
#include "rtthread.h"
#include <rtdevice.h>
#include <rthw.h>
#include <string.h>
#ifdef APP_USING_PAHOMQTT
#ifdef __cplusplus
#include <string>

typedef struct
{
    std::string uri;//������
    int keepalive; //����ʱ��(�������в���Ϊ0��Ĭ��120)
    bool cleansession;//�Ƿ����ɾ��Ự
    std::string clientid;
    struct _auth
    {
        std::string username; //�û���
        std::string password;//����
    } auth;
    struct _will
    {
        std::string will_topic;//��������
        std::string will_payload;//������������
        uint8_t will_qos;//����qos
        bool will_retain;//�����Ƿ�Ϊ������Ϣ
    } will;
    struct _subscribe
    {
        std::string subtopic;//��������
        int qos;//���ĵķ�������
    } subscribe;
} MQTT_Cfg_t;

typedef struct
{
    std::string topic;
    std::string payload;
    uint8_t qos;
    bool retain;
} MQTT_Message_t;

typedef struct
{
    void(*init)(MQTT_Cfg_t &cfg) ;
    void(*connect)(MQTT_Cfg_t &cfg) ;
    void(*disconnect)(MQTT_Cfg_t &cfg) ;
    void(*onmessage)(MQTT_Cfg_t &cfg, MQTT_Message_t &msg);
} MQTT_Callback_t;

/*
��ʼ��MQTT,ֻ�ɵ���һ��
*/
void MQTT_Init();

/*
���ûص�����
*/
void MQTT_Set_Callback(MQTT_Callback_t cb);

/*
������Ϣ
*/
bool MQTT_Publish_Message(MQTT_Message_t &msg);

#endif
#endif
#endif


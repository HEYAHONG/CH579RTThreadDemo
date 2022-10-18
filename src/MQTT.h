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
    std::string uri;//主机名
    int keepalive; //保活时间(本代码中不可为0，默认120)
    bool cleansession;//是否开启干净会话
    std::string clientid;
    struct _auth
    {
        std::string username; //用户名
        std::string password;//密码
    } auth;
    struct _will
    {
        std::string will_topic;//遗嘱主题
        std::string will_payload;//遗嘱负载数据
        uint8_t will_qos;//遗嘱qos
        bool will_retain;//遗嘱是否为保留消息
    } will;
    struct _subscribe
    {
        std::string subtopic;//订阅主题
        int qos;//订阅的服务质量
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
初始化MQTT,只可调用一次
*/
void MQTT_Init();

/*
设置回调函数
*/
void MQTT_Set_Callback(MQTT_Callback_t cb);

/*
发布消息
*/
bool MQTT_Publish_Message(MQTT_Message_t &msg);

#endif
#endif
#endif


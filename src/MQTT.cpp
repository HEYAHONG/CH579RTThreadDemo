#include "MQTT.h"
#ifdef APP_USING_PAHOMQTT

extern "C"
{
#include "paho_mqtt.h"
}

static MQTT_Cfg_t cfg;
static MQTT_Callback_t cfg_cb = {0};
/* define MQTT client context */
static MQTTClient client = {0};

#define MQTT_BUFF_SIZE 512
static uint8_t read_buff[MQTT_BUFF_SIZE];
static uint8_t buff[MQTT_BUFF_SIZE];

static bool check_cfg(MQTT_Cfg_t &Cfg)
{
    if (Cfg.uri.empty())
    {
        return false;
    }

    if (Cfg.keepalive == 0)
    {
        Cfg.keepalive = 120;
    }

    if (Cfg.clientid.empty())
    {
        return false;
    }

    return true;
}

static void mqtt_connect_callback(MQTTClient *c)
{

}

static void mqtt_online_callback(MQTTClient *c)
{
    if (cfg_cb.connect != NULL)
    {
        cfg_cb.connect(cfg);
    }
}

static void mqtt_offline_callback(MQTTClient *c)
{
    if (cfg_cb.disconnect != NULL)
    {
        cfg_cb.disconnect(cfg);
    }

}

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg)
{
    MQTT_Message_t m;
    m.topic = std::string(msg->topicName->lenstring.data, msg->topicName->lenstring.len);
    m.payload = std::string((char *)msg->message->payload, msg->message->payloadlen);
    m.qos = (uint8_t)msg->message->qos;
    m.retain = msg->message->retained;

    if (cfg_cb.onmessage != NULL)
    {
        cfg_cb.onmessage(cfg, m);
    }

}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg)
{
    MQTT_Message_t m;
    m.topic = std::string(msg->topicName->lenstring.data, msg->topicName->lenstring.len);
    m.payload = std::string((char *)msg->message->payload, msg->message->payloadlen);
    m.qos = (uint8_t)msg->message->qos;
    m.retain = msg->message->retained;

    if (cfg_cb.onmessage != NULL)
    {
        cfg_cb.onmessage(cfg, m);
    }
}

static void _thread(void *para)
{
    printf("MQTT Init!!!\r\n");
    while (true)
    {
        if (check_cfg(cfg))
        {
            /* init condata param by using MQTTPacket_connectData_initializer */
            MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
            /* config MQTT context param */
            {
                client.isconnected = 0;
                client.uri = (char *)cfg.uri.c_str();
                /* config connect param */
                rt_memcpy(&client.condata, &condata, sizeof(condata));
                client.condata.clientID.cstring = (char *)cfg.clientid.c_str();
                client.condata.keepAliveInterval = cfg.keepalive;
                client.condata.cleansession = cfg.cleansession;
                client.condata.username.cstring = (char *)cfg.auth.username.c_str();
                client.condata.password.cstring = (char *)cfg.auth.password.c_str();

                if (!cfg.will.will_topic.empty() && !cfg.will.will_payload.empty())
                {
                    /* config MQTT will param. */
                    client.condata.willFlag = 1;
                    client.condata.will.qos = cfg.will.will_qos;
                    client.condata.will.retained = cfg.will.will_retain;
                    client.condata.will.topicName.cstring = (char *)cfg.will.will_topic.c_str();
                    client.condata.will.message.cstring = (char *)cfg.will.will_payload.c_str();
                }


                client.buf_size = client.readbuf_size = MQTT_BUFF_SIZE;
                client.buf = buff;
                client.readbuf = read_buff;


                /* set event callback function */
                client.connect_callback = mqtt_connect_callback;
                client.online_callback = mqtt_online_callback;
                client.offline_callback = mqtt_offline_callback;

                /* set subscribe table and event callback */
                client.messageHandlers[0].topicFilter = (char *)cfg.subscribe.subtopic.c_str() ;
                client.messageHandlers[0].callback = mqtt_sub_callback;
                memcpy(&client.messageHandlers[0].qos, &cfg.subscribe.qos, sizeof(client.messageHandlers[0].qos));

                /* set default subscribe event callback */
                client.defaultMessageHandler = mqtt_sub_default_callback;
            }

            paho_mqtt_start(&client);

            break;

        }
        else
        {
            if (cfg_cb.init != NULL)
            {
                cfg_cb.init(cfg);
            }
        }
        rt_thread_delay(2000);
    }

}

void MQTT_Init()
{
    static bool IsInit = false;
    if (!IsInit)
    {
        rt_thread_t tid = rt_thread_create("MQTT_Start", _thread, NULL, 512, RT_THREAD_PRIORITY_MAX / 3, 2);
        if (tid)
        {
            rt_thread_startup(tid);
        }
    }
}

void MQTT_Set_Callback(MQTT_Callback_t cb)
{
    cfg_cb = cb;
}

bool MQTT_Publish_Message(MQTT_Message_t &msg)
{
    QoS Qos = QOS0;
    switch (msg.qos)
    {
    default:
        break;
    case 0:
        Qos = QOS0;
        break;
    case 1:
        Qos = QOS1;
        break;
    case 2:
        Qos = QOS2;
        break;

    }
    struct MQTTMessage Msg;
    memset(&Msg, 0, sizeof(Msg));
    Msg.payload = (char *)msg.payload.c_str();
    Msg.payloadlen = msg.payload.length();
    Msg.qos = Qos;
    Msg.retained = msg.retain;
    return MQTTPublish(&client, msg.topic.c_str(), &Msg) == 0;
}


#endif


/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-02     Alpha       the first version
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <rtthread.h>
#include "board.h"
#include "drv_common.h"
#include "sensor.h"
#include "sensor_dallas_dht11.h"

#include "cJSON.h"

#define DBG_ENABLE
#define DBG_SECTION_NAME    "mqtt.sample"
#define DBG_LEVEL           DBG_LOG
#define DBG_COLOR

#define THREAD_PRIORITY         10
#define THREAD_STACK_SIZE       8192
#define THREAD_TIMESLICE        5

#define DHT11_DATA_PIN    GET_PIN(A, 0)

#include <rtdbg.h>

#include "paho_mqtt.h"




#include "cJSON.h"

#define MQTT_URI                "tcp://a1w0XJbXwh0.iot-as-mqtt.cn-shanghai.aliyuncs.com:1883"
#define MQTT_CLIENTID           "12345|securemode=3,signmethod=hmacsha1|"
#define MQTT_USERNAME           "SmartLED_01&a1w0XJbXwh0"
#define MQTT_PASSWORD           "ACD00CADFD13AD714E41B5F27898AB349AE1B04D"
#define MQTT_SUBTOPIC           "/sys/a1w0XJbXwh0/SmartLED_01/thing/service/property/set"
#define MQTT_PUBTOPIC           "/sys/a1w0XJbXwh0/SmartLED_01/thing/event/property/post"
#define MQTT_WILLMSG            "Goodbye!"

#define PWM_DEV_NAME        "pwm2"  /* PWM设备名称 */
#define PWM_DEV_CHANNEL     2       /* PWM通道 */

struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */
rt_uint32_t period= 26315;
rt_uint32_t pulse = 8771;


/* define MQTT client context */
static MQTTClient client;
static int is_started = 0;

static u_int16_t ir_open_data[512]={9027,4478,617,1685,616,586,614,586,614,587,614,1686,616,586,614,1687,616,586,613,585,614,1685,617,587,613,1687,617,586,613,586,612,586,587,612,588,612,614,587,613,586,614,585,613,1687,617,1685,618,586,614,586,613,586,613,587,613,587,587,612,613,1687,616,587,612,1688,616,585,613,587,612,1688,617,586,614,20082,565,1716,586,642,558,639,561,639,561,639,560,642,559,642,558,664,537,663,536,639,560,639,561,639,560,641,559,1739,563,638,561,664,536,664,536,663,536,639,560,639,560,638,561,638,561,663,536,639,561,639,561,641,559,641,559,638,560,1740,562,1713,590,1714,588,642,558,10133,8970,4499,588,1736,564,639,560,639,559,641,560,1714,588,638,561,1717,586,637,560,640,561,1738,561,639,560,1715,589,638,561,639,560,639,561,639,561,638,560,638,560,639,560,635,561,1717,585,1714,588,635,562,639,561,638,560,638,562,638,561,635,562,1743,561,1719,581,1716,588,637,561,635,562,1740,562,639,561,20108,590,610,589,610,588,609,590,610,588,611,589,610,589,609,589,610,589,610,589,609,589,611,589,611,589,611,588,611,589,611,589,611,588,611,588,612,588,611,615,584,587,1713,620,581,619,581,615,584,614,584,619,581,619,581,617,582,617,582,618,1679,624,1677,623,580,621,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static u_int16_t ir_close_data[512]={9024,4482,619,1685,620,583,616,584,615,585,616,1685,617,585,615,1686,619,585,589,611,615,1686,617,585,589,1712,593,611,589,612,588,612,588,612,589,612,589,612,589,612,588,612,589,1713,591,1714,592,612,588,612,588,612,588,611,589,611,590,612,589,1713,592,611,589,1712,593,612,589,611,590,1713,591,611,590,20127,624,1663,592,611,590,611,589,611,589,611,590,611,590,611,590,611,589,611,589,610,589,611,590,610,589,611,589,1712,592,628,596,588,588,610,589,611,589,610,589,611,589,611,589,611,589,610,589,610,589,611,589,611,589,611,589,611,588,1713,590,1711,592,1710,593,610,589,10107,8994,4501,624,1680,622,580,619,579,620,580,621,1677,626,578,622,1612,690,577,621,578,621,1675,626,578,620,1680,623,578,621,579,622,578,621,578,621,579,620,579,621,579,620,578,620,1677,626,1677,652,578,595,605,596,604,595,605,594,605,595,603,595,1675,652,1650,651,1651,652,577,621,579,621,1650,650,578,620,20045,627,586,621,578,621,578,621,578,622,576,622,577,621,578,621,578,620,579,621,577,623,577,623,577,622,578,623,576,621,578,622,576,623,577,622,578,623,576,620,578,622,1650,651,578,622,578,621,578,621,579,622,577,622,577,620,578,621,578,623,1649,651,1651,653,577,622,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static u_int16_t ir_cold_data[512]={9025,4495,624,1650,625,605,612,588,593,1677,627,1673,654,579,594,1675,653,577,620,578,621,1650,653,577,619,1650,653,578,623,577,622,577,595,604,622,576,622,576,620,578,622,577,621,577,620,1650,652,577,623,577,621,577,620,578,621,578,623,575,622,1646,654,1650,653,1648,652,579,622,576,621,1650,652,577,621,20042,625,585,621,578,595,604,621,576,621,578,620,579,621,578,621,577,621,578,620,578,621,576,595,605,620,578,620,579,622,577,620,577,596,602,621,578,622,577,621,578,621,1649,652,578,621,578,620,578,621,578,621,578,593,606,621,578,622,577,620,1650,652,1649,652,1648,653,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static u_int16_t ir_hot_data[512]={8998,4500,620,584,615,584,615,1684,618,1685,618,585,615,584,615,584,614,586,615,584,616,584,612,1687,617,1686,617,586,614,586,614,586,614,585,613,586,613,586,588,613,613,586,613,586,614,1686,616,587,614,586,614,585,614,585,613,586,615,585,614,1687,615,586,614,1687,616,585,614,587,614,1685,617,586,614,20087,592,594,614,586,614,586,615,585,614,585,614,586,613,586,615,585,614,584,614,586,588,611,615,585,614,586,614,1685,618,585,613,586,615,585,615,585,615,584,615,585,616,584,614,585,615,584,615,585,615,585,615,584,614,584,615,583,614,585,615,585,615,1683,619,584,614,10059,9022,4474,618,584,615,583,616,1683,618,1684,616,584,588,611,614,585,589,609,615,583,615,583,615,1681,619,1683,617,584,615,584,615,584,616,583,614,586,615,584,615,585,589,611,615,585,615,1683,619,583,617,583,615,583,615,584,590,610,616,584,615,1684,619,1683,619,1683,618,584,613,586,588,1710,620,583,614,20077,592,594,614,584,613,586,587,612,614,586,614,585,613,586,613,585,614,586,588,611,613,585,614,585,613,585,613,585,614,584,614,585,613,585,614,586,613,586,614,586,612,586,614,586,588,610,613,587,613,587,613,586,613,587,612,586,614,585,612,1685,616,586,612,585,612,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};


void ir_send_data(u_int16_t * ir_data)
{
    int i;
    int j;
    int haomiao,weimiao;

    for (i = 0; i < 512; i++) {
             //LOG_D("success");
        if (i%2) {

            rt_pwm_disable(pwm_dev, PWM_DEV_CHANNEL);
            //LOG_D("success");
            haomiao = ir_data[i]/1000;
            weimiao = ir_data[i]%1000;

            for(j=0;j<haomiao;j++)
            {
                rt_hw_us_delay(500);
                rt_hw_us_delay(500);
            }
            //rt_thread_delay(haomiao);
            rt_hw_us_delay(weimiao);
        }
        else {
            rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
            //LOG_D("success");
            haomiao = ir_data[i]/1000;
                        weimiao = ir_data[i]%1000;
                        for(j=0;j<haomiao;j++)
                                    {
                                        rt_hw_us_delay(500);
                                        rt_hw_us_delay(500);
                                    }
                        //rt_thread_delay(haomiao);
                        rt_hw_us_delay(weimiao);
            //rt_hw_us_delay(ir_data[i]);
        }
    }LOG_D("success");
    rt_pwm_disable(pwm_dev, PWM_DEV_CHANNEL);
}

static void mqtt_connect_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_connect_callback!");
}

static void mqtt_online_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_online_callback!");
}

static void mqtt_offline_callback(MQTTClient *c)
{
    LOG_D("inter mqtt_offline_callback!");
}

static void mqtt_sub_callback(MQTTClient *c, MessageData *msg_data)
{
    uint8_t display_num;
    *((char *)msg_data->message->payload + msg_data->message->payloadlen) = '\0';
    //LOG_D("mqtt sub callback:%s",(char *)msg_data->message->payload);

    cJSON* cjson_test = NULL;
    cJSON* cjson_items = NULL;
    cJSON* cjson_display = NULL;
    cJSON* cjson_display_value = NULL;
    cjson_test = cJSON_Parse((char *)msg_data->message->payload);
        if(cjson_test == NULL)
        {
            LOG_D("parse fail.\n");
            return;
        }

        cjson_items = cJSON_GetObjectItem(cjson_test,"items");
        cjson_display = cJSON_GetObjectItem(cjson_items,"LEDSwitch");
        cjson_display_value = cJSON_GetObjectItem(cjson_display,"value");
        display_num = cjson_display_value->valueint;

        cJSON_Delete(cjson_test);

        LOG_D("display number:%d",display_num);

        if (display_num==0) {
            ir_send_data(ir_open_data);
        }

        if (display_num==1) {
            ir_send_data(ir_close_data);
        }

        if (display_num==2) {
            ir_send_data(ir_cold_data);
        }

        if (display_num==3) {
            ir_send_data(ir_hot_data);
        }


}

static void mqtt_sub_default_callback(MQTTClient *c, MessageData *msg_data)
{

}


int mqtt_start()
{
    /* init condata param by using MQTTPacket_connectData_initializer */
    MQTTPacket_connectData condata = MQTTPacket_connectData_initializer;
    static char cid[20] = { 0 };



    if (is_started)
    {
        LOG_E("mqtt client is already connected.");
        return -1;
    }
    /* config MQTT context param */
    {
        client.isconnected = 0;
        client.uri = MQTT_URI;


        rt_snprintf(cid, sizeof(cid), "rtthread%d", rt_tick_get());
                /* config connect param */
        memcpy(&client.condata, &condata, sizeof(condata));


        /* generate the random client ID */
        client.condata.MQTTVersion = 3;
        client.condata.clientID.cstring = MQTT_CLIENTID;
        client.condata.keepAliveInterval = 30;
        client.condata.cleansession = 1;
        client.condata.username.cstring = MQTT_USERNAME;
        client.condata.password.cstring = MQTT_PASSWORD;

        /* config MQTT will param. */
        client.condata.willFlag = 1;
        client.condata.will.qos = 1;
        client.condata.will.retained = 0;
        client.condata.will.topicName.cstring = MQTT_PUBTOPIC;
        client.condata.will.message.cstring = MQTT_WILLMSG;

        /* malloc buffer. */
        client.buf_size = client.readbuf_size = 4096;
        client.buf = rt_calloc(1, client.buf_size);
        client.readbuf = rt_calloc(1, client.readbuf_size);
        if (!(client.buf && client.readbuf))
        {
            LOG_E("no memory for MQTT client buffer!");
            return -1;
        }

        /* set event callback function */
        client.connect_callback = mqtt_connect_callback;
        client.online_callback = mqtt_online_callback;
        client.offline_callback = mqtt_offline_callback;

        /* set subscribe table and event callback */
        client.messageHandlers[0].topicFilter = rt_strdup(MQTT_SUBTOPIC);
        client.messageHandlers[0].callback = mqtt_sub_callback;
        client.messageHandlers[0].qos = QOS1;

        /* set default subscribe event callback */
        client.defaultMessageHandler = mqtt_sub_default_callback;
    }

    /* run mqtt client */
    paho_mqtt_start(&client);
    is_started = 1;
    LOG_D("mqtt_thread init succeed!");

    return 0;
}



static void mqttpublish_thread_entry(void *parameter)
{
        rt_device_t dev = RT_NULL;
        struct rt_sensor_data sensor_data;
        rt_size_t res;
        rt_uint8_t get_data_freq = 1; /* 1Hz */

        dev = rt_device_find("temp_dht11");
        if (dev == RT_NULL)
        {
            return;
        }

        if (rt_device_open(dev, RT_DEVICE_FLAG_RDWR) != RT_EOK)
        {
            rt_kprintf("open device failed!\n");
            return;
        }

        rt_device_control(dev, RT_SENSOR_CTRL_SET_ODR, (void *)(&get_data_freq));

        while (1)
        {
            res = rt_device_read(dev, 0, &sensor_data, 1);

            if (res != 1)
            {
                rt_kprintf("read data failed! result is %d\n", res);
                rt_device_close(dev);
                return;
            }
            else
            {
                if (sensor_data.data.temp >= 0)
                {
                    uint8_t temp = (sensor_data.data.temp & 0xffff) >> 0;      // get temp
                    uint8_t humi = (sensor_data.data.temp & 0xffff0000) >> 16; // get humi
                    //rt_kprintf("temp:%d, humi:%d\n" ,temp, humi);
                    cJSON * pJsonRoot = NULL;
                    pJsonRoot = cJSON_CreateObject();//
                    cJSON_AddStringToObject(pJsonRoot, "id", "12345");//
                    cJSON_AddStringToObject(pJsonRoot, "method", "thing.event.property.post");//
                    cJSON * pJsonChild = cJSON_CreateObject();//
                    cJSON_AddNumberToObject(pJsonChild, "CurrentHumidity",humi);
                    cJSON_AddNumberToObject(pJsonChild, "CurrentTemperature",temp);
                    cJSON_AddItemToObject(pJsonRoot, "params", pJsonChild);//

                    char * lpJsonStr = cJSON_Print(pJsonRoot);
                    cJSON_Delete(pJsonRoot);//

                    paho_mqtt_publish(&client,QOS1,MQTT_PUBTOPIC,lpJsonStr);

                    free(lpJsonStr);
                }
            }

            rt_thread_delay(5000);
        }
}
int mqttpublish_thread_startup(void)
{

    static rt_thread_t tid1 = RT_NULL;

    tid1 = rt_thread_create("mqttpublish_thread",
                            mqttpublish_thread_entry, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

            pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
            if (pwm_dev == RT_NULL)
            {
                rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
                return RT_ERROR;
            }

            /* 设置PWM周期和脉冲宽度默认值 */
            rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);
           // rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
    return 0;
}



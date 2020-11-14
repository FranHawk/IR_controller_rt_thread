/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-10-20     RT-Thread    first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "board.h"
#include "drv_common.h"
#include "sensor.h"
#include "sensor_dallas_dht11.h"
#include "mqtt_aliyun_thread.h"

#define DHT11_DATA_PIN    GET_PIN(A, 0)
#define PWM_DEV_NAME        "pwm2"  /* PWM设备名称 */
#define PWM_DEV_CHANNEL     2       /* PWM通道 */





#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

int main(void)
{
    int count = 1;

    struct rt_device_pwm *pwm_dev;      /* PWM设备句柄 */
    rt_uint32_t period, pulse;
    period = 26315;    /* 周期为0.5ms，单位为纳秒ns */
    pulse = 8771;

    mqtt_start();
    mqttpublish_thread_startup();
//    rt_thread_mdelay(3000);
//    struct rt_sensor_config cfg;
//
//    cfg.intf.user_data = (void *)DHT11_DATA_PIN;
//    rt_hw_dht11_init("dht11", &cfg);

        pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
        if (pwm_dev == RT_NULL)
        {
            rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
            return RT_ERROR;
        }

        /* 设置PWM周期和脉冲宽度默认值 */
        rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);
        /* 使能设备 */
        //rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);


    while (count++)
    {
//        if(count%2)
//        {
//            rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);
//        }
//        else {
//            rt_pwm_disable(pwm_dev, PWM_DEV_CHANNEL);
//        }
        //LOG_D("Hello RT-Thread!");
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}

#include <rtdef.h>
#include <rtthread.h>
#include "sadc.h"

#define CHANNEL_NUM 1
#define REF_VOLT 3300
#define DIGIT_MAX 0x3ff  /// driver def

void sadc_demo_main(void* param)
{
    rt_device_t sadc_dev;

    SADC_INFO info;

    info.channel   = CHANNEL_NUM;
    info.sadc_data = 0;

    sadc_dev = rt_device_find("sadc");

    if (!sadc_dev)
    {
        rt_kprintf("cann't find the sadc dev\n");
    }

    sadc_dev->init(sadc_dev);
    sadc_dev->open(sadc_dev, 0);

    rt_kprintf("channel:%d,ref volt:%dmv\n", info.channel, REF_VOLT);

    while (1)
    {
        sadc_dev->control(sadc_dev, SADC_CMD_READ_RAW_DATA,
                          &info);  ////get digit data

        rt_kprintf("sadc digit:%4d volt:%4dmv\r", info.sadc_data,
                   info.sadc_data * REF_VOLT / DIGIT_MAX);

        rt_thread_delay(100);
    }

    return;
}

void sadc_demo_init()
{
    rt_thread_t threadSadc;

    threadSadc =
        rt_thread_create("sadc", sadc_demo_main, RT_NULL, 10 * 1024, 80, 20);

    if (threadSadc != RT_NULL) rt_thread_startup(threadSadc);
}

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(sadc_demo_main, sadc_demo_main());
#endif

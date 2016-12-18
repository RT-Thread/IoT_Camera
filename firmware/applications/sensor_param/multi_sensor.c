#include <stdlib.h>
#include <stdio.h>
#include <rtthread.h>
#include "multi_sensor.h"

#ifdef FH_USING_MULTI_SENSOR

#ifdef RT_USING_IMX138
extern char ISP_PARAM_BUFF(imx138)[FH_ISP_PARA_SIZE];
#endif

#ifdef RT_USING_OV9712
extern char ISP_PARAM_BUFF(ov9712)[FH_ISP_PARA_SIZE];
#endif

#ifdef RT_USING_OV9732
extern char ISP_PARAM_BUFF(ov9732)[FH_ISP_PARA_SIZE];
#endif

#ifdef RT_USING_AR0130
extern char ISP_PARAM_BUFF(ar0130)[FH_ISP_PARA_SIZE];
#endif

#ifdef RT_USING_H42
extern char ISP_PARAM_BUFF(h42)[FH_ISP_PARA_SIZE];
#endif

#ifdef RT_USING_GC1024
extern char ISP_PARAM_BUFF(gc1024)[FH_ISP_PARA_SIZE];
#endif

#ifdef RT_USING_GC1004  // same with GC1024
extern char ISP_PARAM_BUFF(gc1024)[FH_ISP_PARA_SIZE];
#endif

#ifdef RT_USING_SC1020
extern char ISP_PARAM_BUFF(sc1020)[FH_ISP_PARA_SIZE];
#endif

#ifdef RT_USING_BF3016
extern char ISP_PARAM_BUFF(bf3016)[FH_ISP_PARA_SIZE];
#endif

#ifdef RT_USING_GC0308
extern char ISP_PARAM_BUFF(gc0308)[FH_ISP_PARA_SIZE];
#endif

#define ADD_SENSOR_INFO(suffix)                  \
    {                                            \
        .sensor_name = ""#suffix,                \
        .sensor_param = ISP_PARAM_BUFF(suffix),  \
        .param_len = FH_ISP_PARA_SIZE,           \
    }

FHADV_ISP_SENSOR_INFO_t g_sensor_infos[] = {
#ifdef RT_USING_IMX138
    ADD_SENSOR_INFO(imx138),
#endif
#ifdef RT_USING_OV9712
    ADD_SENSOR_INFO(ov9712),
#endif
#ifdef RT_USING_OV9732
    ADD_SENSOR_INFO(ov9732),
#endif
#ifdef RT_USING_AR0130
    ADD_SENSOR_INFO(ar0130),
#endif
#ifdef RT_USING_H42
    ADD_SENSOR_INFO(h42),
#endif
#ifdef RT_USING_GC1024
    ADD_SENSOR_INFO(gc1024),
#endif
#ifdef RT_USING_SC1020
    ADD_SENSOR_INFO(sc1020),
#endif
#ifdef RT_USING_BF3016
    ADD_SENSOR_INFO(bf3016),
#endif
#ifdef RT_USING_GC0308
    ADD_SENSOR_INFO(gc0308),
#endif
};

#else

extern char isp_param_buff[FH_ISP_PARA_SIZE];

FHADV_ISP_SENSOR_INFO_t g_sensor_infos[] = {
    {
        .sensor_name = "fixed",
        .sensor_param = isp_param_buff,
        .param_len = FH_ISP_PARA_SIZE
    }
};

#endif

void get_isp_sensor_info(FHADV_ISP_SENSOR_INFO_t **info, int *len)
{
    *info = g_sensor_infos;
    *len = sizeof(g_sensor_infos) / sizeof(FHADV_ISP_SENSOR_INFO_t);
}
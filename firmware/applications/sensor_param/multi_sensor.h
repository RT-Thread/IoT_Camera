#ifndef _MULTI_SENSOR_H
#define _MULTI_SENSOR_H

#include "FHAdv_Isp_mpi.h"

#define FH_ISP_PARA_SIZE (0x000007eC)

#ifdef FH_USING_MULTI_SENSOR
#define ISP_PARAM_BUFF(n) isp_param_buff_##n
#endif

void get_isp_sensor_info(FHADV_ISP_SENSOR_INFO_t **info, int *len);

#endif

/**
 * @file     FHAdv_Isp_mpi.h
 * @brief    FHAdv ISP moduel interface
 * @version  V1.0.0
 * @date     20-July-2016
 * @author   Software Team
 *
 * @note
 * Copyright (C) 2016 Shanghai Fullhan Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * @par
 * Fullhan is supplying this software which provides customers with programming
 * information regarding the products. Fullhan has no responsibility or
 * liability for the use of the software. Fullhan not guarantee the correctness
 * of this software. Fullhan reserves the right to make changes in the software
 * without notification.
 *
 */

#ifndef _FHADV_ISP_MPI_H
#define _FHADV_ISP_MPI_H

#include "FH_typedef.h"

/** @addtogroup Video_ISP 图像效果调整接口
 *  @{
 */

enum FH_SINT32t
{
    FH_SINT32t_1_3,
    FH_SINT32t_1_6,
    FH_SINT32t_1_12,
    FH_SINT32t_1_25,
    FH_SINT32t_1_50,
    FH_SINT32t_1_100,
    FH_SINT32t_1_150,
    FH_SINT32t_1_200,
    FH_SINT32t_1_250,
    FH_SINT32t_1_500,
    FH_SINT32t_1_750,
    FH_SINT32t_1_1000,
    FH_SINT32t_1_2000,
    FH_SINT32t_1_4000,
    FH_SINT32t_1_10000,
    FH_SINT32t_1_100000,
};
typedef enum FH_SINT32t FHADV_ISP_AE_TIME_LEVEL;

/**
 *  @brief      Sensor信息结构体
 */
typedef struct {
    FH_SINT8 *sensor_name;
    FH_SINT8 *sensor_param;
    FH_SINT32 param_len;
    void *sensor_handle;
} FHADV_ISP_SENSOR_INFO_t;

/**
 *  @brief      Sensor自动探测结构体
 */
typedef struct
{
    FHADV_ISP_SENSOR_INFO_t *sensor_infos;
    FH_SINT32 sensor_num;
} FHADV_ISP_SENSOR_PROBE_INFO_t;

/**
 *  @brief      sensor初始化
 *
 * @param[in]   probe_info
 * @param[out]  probed_sensor   探测到的sensor信息结构体
 * @return      0 或 -1
 */
FH_SINT32 FHAdv_Isp_SensorInit(FHADV_ISP_SENSOR_PROBE_INFO_t *probe_info, FHADV_ISP_SENSOR_INFO_t *probed_sensor);

/**
 *  @brief      ISP功能初始化
 *
 *              为了能正确读取初始参数，请务必在程序初始化时调用FHAdv_Isp_Init接口。
 *
 * @param[in]   无
 * @return      0
 */
FH_SINT32 FHAdv_Isp_Init(void);

/**
 *  @brief      设置自动白平衡增益
 *
 * @param[in]   mode    设置模式：
 *                      mode 0:自动白平衡
 *                      mode 1:锁定白平衡
 *                      mode 2:白炽灯
 *                      mode 3:暖光灯
 *                      mode 4:自然光
 *                      mode 5:日光灯
 * @return      0 或 -1
 */
FH_SINT32 FHAdv_Isp_SetAwbGain(FH_SINT32 mode);

/**
 *  @brief      获取自动白平衡增益
 *
 *              使用方法参考：
 *                 FH_UINT32 awbgain[4];
 *                 FHAdv_Isp_GetAwbGain(awbgain);
 *              结果保存在awbgain数组中
 *
 * @param[out]  AwbGain    数组首地址
 * @return      0
 */
FH_SINT32 FHAdv_Isp_GetAwbGain(FH_UINT32 *AwbGain);

/**
 *  @brief      设置自动曝光模式
 *
 * @param[in]   mode               设置模式,或者直接分为两种模式（手动和自动）:
 *                                 mode 0:自动
 *                                 mode 1:曝光时间手动
 *                                 mode 2:曝光增益手动
 *                                 mode 3:均手动
 * @param[in]   ae_time_level      FHADV_ISP_AE_TIME_LEVEL枚举类型中的一个，或 0 ~ 15
 * @param[in]   gain_level         增益等级，0 ~ 100
 * @return      0
 */
FH_UINT32 FHAdv_Isp_SetAEMode(FH_SINT32 mode, FH_SINT32 ae_time_level, FH_SINT32 gain_level);

/**
 *  @brief      获取自动曝光模式
 *
 *              使用方法参考：
 *                 FH_SINT32 FH_SINT32t, gain_value;
 *                 FHAdv_Isp_GetAEMode(&FH_SINT32t, &gain_value);
 *
 * @param[out]  ae_lines    实际曝光行数
 * @param[out]  gain_value  实际曝光增益值
 * @return      0
 */
FH_UINT32 FHAdv_Isp_GetAEMode(FH_UINT32 *ae_lines, FH_UINT32 *gain_value);

/**
 *  @brief      设置宽动态参数
 *
 * @param[in]   mode    0:关闭, 1:开启
 * @param[in]   level   0~10 值越大，动态范围越大
 * @return      0 或 -1
 */
FH_UINT32 FHAdv_Isp_SetLtmCfg(FH_SINT32 mode, FH_SINT32 level);

/**
 *  @brief      设置宽动态参数
 *
 *              使用参考：
 *                 FH_SINT32 mode,level;
 *                 FHAdv_Isp_GetLtmCfg(&mode,&level);
 *
 * @param[out]  mode
 * @param[out]  level
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_UINT32 FHAdv_Isp_GetLtmCfg(FH_SINT32 *mode, FH_SINT32 *level);

/**
 *  @brief      设置亮度，参考亮度设置没有手动模式
 *
 * @param[in]   value    范围0-255 建议参考值范围0-240
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_SetBrightness(FH_UINT32 value);

/**
 *  @brief      获取当前亮度值
 *
 * @param[out]  pValue
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_GetBrightness(FH_UINT32 *pValue);

/**
 *  @brief      设置对比度
 *
 * @param[in]   mode    0:手动模式，1:gainmapping模式，根据mapping选择并可以手动做整体mapping调整
 * @param[in]   value   范围0-255 建议参考值范围：0-40 （最大值一般为默认值2倍即可）
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_SetContrast(FH_SINT32 mode, FH_UINT32 value);

/**
 *  @brief      获取当前对比度
 *
 * @param[out]  mode
 * @param[out]  pValue
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_GetContrast(FH_SINT32 *mode, FH_UINT32 *pValue);

/**
 *  @brief      设置饱和度
 *
 * @param[in]   mode    0:手动模式，1:gainmapping模式，根据mapping选择并可以手动做整体mapping调整
 * @param[in]   value   范围0-255 建议参考值范围：0-200 （最大值一般为默认值2倍即可）
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_SetSaturation(FH_SINT32 mode, FH_UINT32 value);

/**
 *  @brief      获取当前饱和度
 *
 * @param[out]  mode
 * @param[out]  pValue
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_GetSaturation(FH_SINT32 *mode, FH_UINT32 *pValue);

/**
 *  @brief      设置锐度
 *
 * @param[in]   mode    0:手动模式，1:gainmapping模式，根据mapping选择并可以手动做整体mapping调整
 * @param[in]   value   范围0-255 建议参考值范围：0-50（最大值一般为默认值2倍即可）
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_SetSharpeness(FH_SINT32 mode, FH_UINT32 value);

/**
 *  @brief      获取当前锐度
 *
 * @param[out]  mode
 * @param[out]  pValue
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_GetSharpeness(FH_SINT32 *mode, FH_UINT32 *pValue);

/**
 *  @brief      设置色度
 *
 *              暂未实现
 *
 * @param[in]   value
 * @return      0
 */
FH_SINT32 FHAdv_Isp_SetChroma(FH_UINT32 value);

/**
 *  @brief      获取色度
 *
 *              暂未实现
 *
 * @param[out]  pValue
 * @return      0
 */
FH_SINT32 FHAdv_Isp_GetChroma(FH_UINT32 *pValue);

/**
 *  @brief      设置镜像和翻转
 *
 * @param[in]   mirror    0:恢复镜像，1:使能镜像
 * @param[in]   flip      0:恢复翻转，1:使能翻转
 * @return      0
 */
FH_SINT32 FHAdv_Isp_SetMirrorAndflip(FH_SINT32 mirror, FH_SINT32 flip);

/**
 *  @brief      获取镜像和翻转的状态
 *
 * @param[out]  pMirror
 * @param[out]  pFlip
 * @return      0
 */
FH_SINT32 FHAdv_Isp_GetMirrorAndflip(FH_SINT32 *pMirror, FH_SINT32 *pFlip);

/**
 *  @brief      设置颜色模式
 *
 * @param[in]   mode    0:饱和度为0，1:使用默认饱和度
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_SetColorMode(FH_SINT32 mode);

/**
 *  @brief      获取颜色模式
 *
 * @param[out]  pMode
 * @return      0 或 小于0的错误值
 */
FH_SINT32 FHAdv_Isp_GetColorMode(FH_SINT32 *pMode);

/*@} end of group Video_ISP */

#endif

/**
 * @file     FHAdv_video_md.h
 * @brief    FHAdv video motion detect, cover detect module interface
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

#ifndef _FHADV_VIDEO_MD_H
#define _FHADV_VIDEO_MD_H

#include "FH_typedef.h"

/** @addtogroup Video_Md Video Motion Detect
 *  @{
 */

#define MD_AREA_MAX_NUM 32

/**
*  @brief 矩形区域信息结构体
*/
typedef struct
{
    int fTopLeftX; /*!< 矩形区域左上点水平方向上的坐标 */
    int fTopLeftY; /*!< 矩形区域左上点垂直方向上的坐标 */
    int fWidth;    /*!< 矩形区域的宽度 */
    int fHeigh;    /*!< 矩形区域的高度 */
} FHT_Rect_t;

/**
*  @brief 移动侦测配置结构体
*/
typedef struct
{
    FH_BOOL enable;
    FH_UINT32 threshold;  /*!< 灵敏度 (0~255) */
    FH_UINT32 framedelay; /*!< 帧间隔，即每隔几帧计算一次 */

    FHT_Rect_t md_area[MD_AREA_MAX_NUM]; /*!< 移动侦测检测区域 */
} FHT_MDConfig_t;

/**
*  @brief 移动侦测扩展功能配置结构体，针对全幅面区域进行检测，
*         故此处不需要配置检测区域
*/
typedef struct
{
    FH_BOOL enable;
    FH_UINT32 threshold;  /*!< 灵敏度 (0~255) */
    FH_UINT32 framedelay; /*!< 帧间隔，即每隔几帧计算一次 */
} FHT_MDConfig_Ex_t;

/**
*  @brief 遮挡侦测灵敏度级别
*/
typedef enum {
    CD_LEVEL_LOW = 1,
    CD_LEVEL_MID,
    CD_LEVEL_HIGH,
} FHE_CD_LEVEL;

/**
*  @brief 遮挡侦测配置结构体
*/
typedef struct
{
    FH_BOOL enable;
    FH_UINT32 framedelay;  /*!< 帧间隔，即每隔几帧计算一次 */
    FHE_CD_LEVEL cd_level; /*!< 灵敏度级别 */
} FHT_CDConfig_t;

/**
*  @brief 移动侦测扩展功能检测结果结构体
*
*         扩展功能是将整个幅面划分成多个宏块，计算每个宏块中是否有物体移动，
*         因此检测结果是一个数组，每个数组元素对应1个宏块，所以总的宏块个数
*         为 horizontal_count * vertical_count
*/
typedef struct
{
    FH_UINT8 *start;            /*!< 数据起始地址，每个数据长度为1个字节 */
    FH_UINT32 horizontal_count; /*!< 水平方向上宏块个数*/
    FH_UINT32 vertical_count;   /*!< 垂直方向上宏块个数 */
} FHT_MDConfig_Ex_Result_t;

/**
 *  @brief      移动侦测功能初始化
 *
 * @param[in]   无
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_MD_Init(void);

/**
 *  @brief      设置移动侦测参数
 *
 * @param[in]   md_config    FHT_MDConfig_t结构体指针
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_MD_SetConfig(FHT_MDConfig_t *md_config);

/**
 *  @brief      获取已经配置的移动侦测参数
 *
 * @param[out]  md_config    FHT_MDConfig_t结构体指针
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_MD_GetConfig(FHT_MDConfig_t *md_config);

/**
 *  @brief      获取移动侦测检测结果
 *
 *              检测结果为32位数据，每个bit对应一个检测区域的结果，
 *              0为没有检测到移动，1为检测到移动；最低位对应区域数
 *              组md_area[0]，最高位对应区域数组md_area[31]
 *
 * @param[out]  md_result    保存结果的数据地址
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_MD_GetResult(FH_UINT32 *md_result);

/**
 *  @brief      移动侦测扩展功能初始化
 *
 * @param[in]   无
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_MD_Ex_Init(void);

/**
 *  @brief      设置移动侦测扩展功能参数
 *
 * @param[in]   md_ex_config    FHT_MDConfig_Ex_t结构体指针
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_MD_Ex_SetConfig(FHT_MDConfig_Ex_t *md_ex_config);

/**
 *  @brief      获取已经配置的移动侦测扩展功能参数
 *
 * @param[out]  md_ex_config    FHT_MDConfig_Ex_t结构体指针
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_MD_Ex_GetConfig(FHT_MDConfig_Ex_t *md_ex_config);

/**
 *  @brief      获取移动侦测扩展功能检测结果
 *
 *              扩展功能针对整个幅面区域进行检测，将整个幅面以32x32的宏块进行划分，
 *              检测结果保存在一个元素为8bit的数组中，每个元素对应一个32x32宏块的
 *              移动侦测结果
 *
 * @param[out]  md_ex_result    FHT_MDConfig_Ex_Result_t结构体指针
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_MD_Ex_GetResult(FHT_MDConfig_Ex_Result_t *md_ex_result);

/**
 *  @brief      进行移动侦测、遮挡侦测的计算，需要不断调用
 *
 * @param[in]   无
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_MD_CD_Check(void);

/**
 *  @brief      遮挡侦测初始化
 *
 * @param[in]   无
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_CD_Init(void);

/**
 *  @brief      设置遮挡侦测参数
 *
 * @param[in]   cd_config    FHT_CDConfig_t结构体指针
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_CD_SetConfig(FHT_CDConfig_t *cd_config);

/**
 *  @brief      获取已经配置的遮挡侦测参数
 *
 * @param[out]  cd_config    FHT_CDConfig_t结构体指针
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_CD_GetConfig(FHT_CDConfig_t *cd_config);

/**
 *  @brief      获取遮挡侦测检测结果
 *
 *              检测结果为0表示没有检测到物体遮挡，1为检测到遮挡
 *
 *
 * @param[out]  cd_result    保存结果的数据地址
 * @return      FH_SUCCESS或FH_FAILURE
 */
FH_SINT32 FHAdv_CD_GetResult(FH_UINT32 *cd_result);

/*@} end of group Video_Md */

#endif
/*
 * File      : fh_audio_mpi.h
 * This file is part of FH8620 BSP for RT-Thread distribution.
 *
 * Copyright (c) 2016 Shanghai Fullhan Microelectronics Co., Ltd.
 * All rights reserved
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  Visit http://www.fullhan.com to get contact with Fullhan.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2016.2.29      xuww         first version
 */

#include "type_def.h"
#include "fh_audio_mpipar.h"

//#define FH_ACW_MPI_DEBUG

#if defined(FH_ACW_MPI_DEBUG) && defined(RT_DEBUG)
#define PRINT_ACW_MPI_DBG(fmt, args...) \
    do                                  \
    {                                   \
        rt_kprintf("FH_MPI_DEBUG: ");   \
        rt_kprintf(fmt, ##args);        \
    } while (0)
#else
#define PRINT_ACW_MPI_DBG(fmt, args...) \
    do                                  \
    {                                   \
    } while (0)
#endif


/**
* FH_AC_DeInit
*@brief 音频系统初始化，完成打开驱动设备
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/

FH_SINT32 FH_AC_Init();

/**
* FH_AC_DeInit
*@brief 音频系统释放资源，完成关闭驱动设备
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_DeInit();

/**
* FH_AC_Set_Config
*@brief 设置AI AO 设备参数
*@param [in] pstConfig 设备参数信息结构体指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_Set_Config(struct fh_audio_cfg_arg *pstConfig);

/**
* FH_AC_AI_Enable
*@brief 设置AI设备运行
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AI_Enable();

/**
* FH_AC_AI_Disable
*@brief 关闭AI 设备运行
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AI_Disable();

/**
* FH_AC_AO_Enable
*@brief 设置AO 设备运行
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AO_Enable();

/**
* FH_AC_AO_Disable
*@brief 关闭AO 设备
*@param [in]  无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AO_Disable();

/**
* FH_AC_AI_Pause
*@brief 暂停AI设备
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AI_Pause();

/**
* FH_AC_AI_Resume
*@brief 恢复AI设备运行
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AI_Resume();

/**
* FH_AC_AO_Pause
*@brief 恢复AI设备运行
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AO_Pause();

/**
* FH_AC_AO_Resume
*@brief 暂停AO设备
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AO_Resume();

/**
* FH_AC_AI_SetVol
*@brief 设置AI设备音量大小
*@param [in] volume 音量大小,0 ~ 100
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AI_SetVol(FH_SINT32 volume);

/**
* FH_AC_AI_GetFrame
*@brief 获取一帧音频数据
*@param [in] 无
*@param [out] pstFrame 音频数据结构提指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AI_GetFrame(FH_AC_FRAME_S *pstFrame);

/**
* FH_AC_AO_SendFrame
*@brief 发送一帧音频数据
*@param [in] pstFrame 音频数据结构体指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0): 成功
* - 其他：失败，错误详见错误号
*/
FH_SINT32 FH_AC_AO_SendFrame(FH_AC_FRAME_S *pstFrame);

int fh_mpi_test();

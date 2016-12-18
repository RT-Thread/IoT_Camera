#ifndef __FH_VOU_MPI_H__
#define __FH_VOU_MPI_H__

#include "fh_vou_mpipara.h"
#include "types/type_def.h"
#include "fh_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
* FH_VOU_Enable
*@brief 启用视频输出设备
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VOU_Enable();

/**
* FH_VOU_Disable
*@brief 禁用视频输出设备
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VOU_Disable();

/**
* FH_VOU_SetConfig
*@brief 设置视频输出设备的配置属性。
*@param [in] pstVouconfig：配置参数指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VOU_SetConfig(const FH_VOU_PIC_CONFIG *pstVouconfig);

/**
* FH_VOU_GetConfig
*@brief 获取视频输出设备的配置属性。
*@param [in] 无
*@param [out] pstVouconfig：配置参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VOU_GetConfig(FH_VOU_PIC_SIZE *pstVouconfig);
/**
* FH_VOU_Enable
*@brief 启用视频输出设备
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VOU_SendFrame(const FH_VOU_PIC_INFO *pstVouframeinfo);

/**
* fh_vou_init
*@brief 视频显示驱动初始化，供内部调用，用户无需显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 fh_vou_init();

/**
* fh_vou_close
*@brief 视频显示驱动退出，供内部调用，用户无需显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 fh_vou_close();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*__MPI_VO_H__ */

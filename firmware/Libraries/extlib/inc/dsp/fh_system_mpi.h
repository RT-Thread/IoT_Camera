#ifndef __FH_SYSTEM_MPI_H__
#define __FH_SYSTEM_MPI_H__

#include "types/type_def.h"
#include "fh_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
* FH_SYS_Init
*@brief DSP 系统初始化，完成打开驱动设备及分配系统内存
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_Init();

/**
* FH_SYS_Exit
*@brief DSP 系统退出
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_Exit();

/**
* FH_SYS_BindVpu2Enc
*@brief 数据源绑定到H264编码通道
*@param [in] srcchn(VPU 通道),取值为0 - 2
*@param [in] dstschn(ENC 通道)，取值为0 - 7
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_BindVpu2Enc(FH_UINT32 srcchn, FH_UINT32 dstschn);

/**
* FH_SYS_BindVpu2Vou
*@brief 数据源绑定到显示通道
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_BindVpu2Vou();

/**
* FH_SYS_BindVpu2Jpeg
*@brief 数据源绑定JPEG编码通道。
*@param [in] srcchn(VPU 通道),取值为0 - 2
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_BindVpu2Jpeg(FH_UINT32 srcchn);

/**
* FH_SYS_GetBindbyDest
*@brief 获取H264编码通道的绑定对象
*@param [in] dstschn(ENC 通道)，取值为0 - 7
*@param [out] srcchn(VPU 通道),取值为0 - 2
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_GetBindbyDest(FH_UINT32 *srcchn, FH_UINT32 dstschn);

/**
* FH_SYS_UnBindbySrc
*@brief 解除数据绑定关系
*@param [in] srcchn(VPU 通道),取值为0 - 2
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_UnBindbySrc(FH_UINT32 srcchn);

/**
* FH_SYS_GetVersion
*@brief 获取DSP驱动版本信息
*@param [in] 无
*@param [out] pstSystemversion 版本信息
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_GetVersion(FH_VERSION_INFO *pstSystemversion);

/**
* FH_SYS_SetReg
*@brief 设置寄存器值
*@param [in] addr 寄存器物理地址
*@param [in] value 设置寄存器值
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_SetReg(FH_UINT32 addr, FH_UINT32 value);

/**
* FH_SYS_GetReg
*@brief 获取寄存器的值
*@param [in] u32Addr 寄存器物理地址
*@param [out] pu32Value 寄存器的值
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_SYS_GetReg(FH_UINT32 u32Addr, FH_UINT32 *pu32Value);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /*__MPI_VO_H__ */

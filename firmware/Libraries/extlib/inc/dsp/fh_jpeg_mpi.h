#ifndef __FH_JPEG_MPI_H__
#define __FH_JPEG_MPI_H__

#include "types/type_def.h"
#include "fh_common.h"
#include "fh_jpeg_mpipara.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
* FH_JPEG_InitMem
*@brief 配置JPEG模块需要的内存
*@param [in] Jpegwidth：JPEG编码的宽度。
*@param [in] Jpeghight：JPEG编码的高度。
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_JPEG_InitMem(FH_UINT32 Jpegwidth, FH_UINT32 Jpeghight);

/**
* FH_JPEG_Setconfig
*@brief 设置JPEG配置参数
*@param [in] pstJpegconfig JPEG编码配置参数
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_JPEG_Setconfig(const FH_JPEG_CONFIG *pstJpegconfig);

/**
* FH_JPEG_Getconfig
*@brief 获取JPEG配置参数
*@param [in] 无
*@param [out] pstJpegconfig：JPEG编码配置参数
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_JPEG_Getconfig(FH_JPEG_CONFIG *pstJpegconfig);

/**
* FH_JPEG_Setqp
*@brief 设置JPEG编码的QP值，动态调整。
*@param [in] QP  JPEG编码的QP值
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_JPEG_Setqp(FH_UINT32 QP);

/**
* FH_JPEG_Getqp
*@brief 获取JPEG编码的QP值。
*@param [in] 无
*@param [out] QP  JPEG编码的QP值
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_JPEG_Getqp(FH_UINT32 *QP);

/**
* FH_JPEG_Setstream
*@brief 用户手动提交一帧图像给JPEG编码
*@param [in] pstJpegframe 编码的码流信息
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_JPEG_Setstream(const FH_JPEG_FRAME_INFO *pstJpegframe);

/**
* FH_JPEG_Getstream
*@brief 获取JPEG编码的码流信息，非阻塞，只有一帧的buf，需要在图像完全取走后，才可以启动下一帧编码。
*@param [in] 无
*@param [out] pstJpegconfig 编码的码流信息
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_JPEG_Getstream(FH_JPEG_STREAM_INFO *pstJpegframe);

/**
* FH_JPEG_Getstream_Block
*@brief 获取JPEG编码的码流信息，阻塞方式，只有一帧的buf，需要在图像完全取走后，才可以启动下一帧编码。
*@param [in] 无
*@param [out] pstJpegconfig 编码的码流信息
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_JPEG_Getstream_Block(FH_JPEG_STREAM_INFO *pstJpegframe);

/**
* fh_jpeg_init
*@brief JPEG驱动初始化，供内部调用，用户无需显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 fh_jpeg_init();

/**
* fh_jpeg_close
*@brief JPEG驱动初始化，供内部调用，用户无需显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 fh_jpeg_close();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /*__MPI_VO_H__ */

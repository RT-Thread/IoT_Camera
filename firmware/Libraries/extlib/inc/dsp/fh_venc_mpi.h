

#ifndef __FH_VENC_MPI_H__
#define __FH_VENC_MPI_H__

#include "fh_venc_mpipara.h"
#include "types/type_def.h"
#include "fh_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
* FH_VENC_SysInitMem
*@brief VENC系统内存初始化，供内部调用，用户不需要显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SysInitMem();

/**
* FH_VENC_CreateChn
*@brief 创建编码通道，配置通道的属性值。
*@param [in] chan 通道号，取值0-7
*@param [out] stVencChnAttr 通道参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_CreateChn(FH_UINT32 chan, const FH_CHR_CONFIG *stVencChnAttr);

/**
* FH_VENC_StartRecvPic
*@brief 开始接收图片输入进行编码。
*@param [in] chan 通道号，取值0-7
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_StartRecvPic(FH_UINT32 chan);

/**
* FH_VENC_StopRecvPic
*@brief 停止编码。
*@param [in] chan 通道号，取值0-7
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_StopRecvPic(FH_UINT32 chan);

/**
* FH_VENC_Submit_ENC
*@brief VENC没有和VPU进行通道绑定，使用用户提供图片进行编码。
*@param [in] chan 通道号，取值0-7
*@param [in] pstVencsubmitframe 用户提供的图片信息指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_Submit_ENC(FH_UINT32 chan, const FH_ENC_FRAME *pstVencsubmitframe);

/**
* FH_VENC_Query
*@brief 查询编码器状态信息。
*@param [in] 无
*@param [out] pstVencstatus 编码状态信息指针。
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_Query(FH_SYS_STATUS *pstVencstatus);

/**
* FH_VENC_GetStream
*@brief 获取编码通道码流。非阻塞
*@param [in] chan 通道号，取值0-7
*@param [out] pstVencstreamAttr 编码通道码流信息指针。
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GetStream(FH_UINT32 chan, FH_ENC_STREAM_ELEMENT *pstVencstreamAttr);

/**
* FH_VENC_ReleaseStream
*@brief 释放码流缓存。
*@param [in] chan 通道号，取值0-7
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_ReleaseStream(FH_UINT32 chan);

/**
* FH_VENC_SetChnAttr
*@brief 设置编码通道属性。通道创建之后可以通过此接口改变通道属性。
*@param [in] chan 通道号，取值0-7
*@param [in] pstVencChnAttr 编码通道参数指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SetChnAttr(FH_UINT32 chan, const FH_CHR_CONFIG *pstVencChnAttr);

/**
* FH_VENC_GetChnAttr
*@brief 获取编码通道的编码属性。
*@param [in] chan 通道号，取值0-7
*@param [out] pstVencChnAttr 编码通道参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GetChnAttr(FH_UINT32 chan, FH_CHR_CONFIG *pstVencChnAttr);

/**
* FH_VENC_SetRotate
*@brief 设置图像旋转。
*@param [in] chan 通道号，取值0-7
*@param [in] pstVencrotateinfo 旋转信息指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SetRotate(FH_UINT32 chan, const FH_ROTATE *pstVencrotateinfo);

/**
* FH_VENC_GetRotate
*@brief 设置图像旋转。
*@param [in] chan 通道号，取值0-7
*@param [out] pstVencrotateinfo 旋转信息指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GetRotate(FH_UINT32 chan, FH_ROTATE *pstVencrotateinfo);

/**
* FH_VENC_SetRoiCfg
*@brief 设置图像旋转。
*@param [in] chan 通道号，取值0-7
*@param [in] pstVencroiinfo ROI信息指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SetRoiCfg(FH_UINT32 chan, const FH_ROI *pstVencroiinfo);

/**
* FH_VENC_GetRoiCfg
*@brief 获取编码图像的ROI信息
*@param [in] chan 通道号，取值0-7
*@param [out] pstVencroiinfo ROI信息指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GetRoiCfg(FH_UINT32 chan, FH_ROI *pstVencroiinfo);

/**
* FH_VENC_ClearRoi
*@brief 清除编码通道的ROI信息，在第一次设置通道ROI或需要清除旧的设置时调用
*@param [in] chan 通道号，取值0-7
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_ClearRoi(FH_UINT32 chan);

/**
* FH_VENC_SetH264eRefMode
*@brief 设置H.264 编码通道跳帧参考模式。
*@param [in] chan 通道号，取值0-7
*@param [in] Vencreferencemode 跳帧选项
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SetH264eRefMode(FH_UINT32 chan, FH_REF_MODE_OPS Vencreferencemode);

/**
* FH_VENC_GetH264eRefMode
*@brief 获取H.264 编码通道跳帧参考模式
*@param [in] chan 通道号，取值0-7
*@param [out] pVencreferencemode 跳帧选项
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GetH264eRefMode(FH_UINT32 chan, FH_REF_MODE_OPS *pVencreferencemode);

/**
* FH_VENC_SetH264RcIDivP
*@brief 设置H.264 编码通道码流控制时的I帧P帧比例
*@param [in] chan 通道号，取值0-7
*@param [in] pVencrcidivp I帧P帧比例的参数指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SetH264RcIDivP(FH_UINT32 chan, const FH_RC_I_DIV_P *pVencrcidivp);

/**
* FH_VENC_GetH264RcIDivP
*@brief 获取H.264 编码通道码流控制时的I帧P帧比例
*@param [in] chan 通道号，取值0-7
*@param [out] pVencrcidivp 熵编码的参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GetH264RcIDivP(FH_UINT32 chan, FH_RC_I_DIV_P *pVencrcidivp);

/**
* FH_VENC_SetH264RcFluctateLevel
*@brief 设置H.264 编码通道码流控制时码流波动等级
*@param [in] chan 通道号，取值0-7
*@param [out] fluctuate_level 码流波动等级0 - 6,越小波动越大，图像质量变化越平稳
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SetH264RcFluctateLevel(FH_UINT32 chan, FH_UINT32 fluctuate_level);

/**
* FH_VENC_GetH264RcFluctateLevel
*@brief 获取H.264 编码通道码流控制时码流波动等级
*@param [in] chan 通道号，取值0-7
*@param [out] fluctuate_level 码流波动等级0 - 6,越小波动越大，图像质量变化越平稳?
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GetH264RcFluctateLevel(FH_UINT32 chan, FH_UINT32 *fluctuate_level);

/**
* FH_VENC_SetH264Entropy
*@brief 设置H.264 编码通道熵编码模式
*@param [in] chan 通道号，取值0-7
*@param [in] pstVencentropy 熵编码的参数指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SetH264Entropy(FH_UINT32 chan, const FH_ENTROPY_OPS *pstVencentropy);

/**
* FH_VENC_GetH264Entropy
*@brief 获取H.264 编码通道熵编码模式
*@param [in] chan 通道号，取值0-7
*@param [out] pstVencentropy 熵编码的参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GetH264Entropy(FH_UINT32 chan, FH_ENTROPY_OPS *pstVencentropy);

/**
* FH_VENC_SET_ADV_DEBLOCKING_FILTER
*@brief 设置H.264 编码通道环路滤波参数
*@param [in] chan 通道号，取值0-7
*@param [in] pstVencentropy 环路滤波参数指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SET_ADV_DEBLOCKING_FILTER(FH_UINT32 chan, const FH_DEBLOCKING_FILTER_PARAM *pstVencfilter);

/**
* FH_VENC_GET_ADV_DEBLOCKING_FILTER
*@brief 获取H.264 编码通道环路滤波参数
*@param [in] chan 通道号，取值0-7
*@param [out] pstVencentropy 环路滤波参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GET_ADV_DEBLOCKING_FILTER(FH_UINT32 chan, FH_DEBLOCKING_FILTER_PARAM *pstVencfilter);

/**
* FH_VENC_SET_ADV_INTERMBSCE
*@brief 设置H.264 编码通道单系数消除模式。
*@param [in] chan 通道号，取值0-7
*@param [in] Vencintermbsce 单系数消除选项
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SET_ADV_INTERMBSCE(FH_UINT32 chan, FH_INTERMBSCE_OPS Vencintermbsce); /*设置单系数消除*/

/**
* FH_VENC_GET_ADV_INTERMBSCE
*@brief 获取H.264 编码通道单系数消除模式。
*@param [in] chan 通道号，取值0-7
*@param [out] pVencintermbsce 环路滤波参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GET_ADV_INTERMBSCE(FH_UINT32 chan, FH_INTERMBSCE_OPS *pVencintermbsce);

/**
* FH_VENC_SET_ADV_SLICE_SPLIT
*@brief 设置H.264 编码通道Slice分割
*@param [in] chan 通道号，取值0-7
*@param [in] pstVencslicesplit Slice分割属性值
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SET_ADV_SLICE_SPLIT(FH_UINT32 chan, const FH_SLICE_SPLIT *pstVencslicesplit); /*设置Slice分割*/

/**
* FH_VENC_GET_ADV_SLICE_SPLIT
*@brief 获取H.264 编码通道Slice分割
*@param [in] chan 通道号，取值0-7
*@param [out] pstVencslicesplit Slice分割属性值
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GET_ADV_SLICE_SPLIT(FH_UINT32 chan, FH_SLICE_SPLIT *pstVencslicesplit);

/**
* FH_VENC_RequestIDR
*@brief 请求强制I 帧。
*@param [in] chan 通道号，取值0-7
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_RequestIDR(FH_UINT32 chan);

/**
* FH_VENC_SetCurPts
*@brief 设置PTS的时间，供内部调用，用户无需显性调用
*@param [in] Systemstartpts 设置的PTS值
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_SetCurPts(FH_UINT64 Systemstartpts);

/**
* 获取当前的系统PTS值
*@brief 设置PTS的时间，供内部调用，用户无需显性调用
*@param [in] 无
*@param [out] Systemcurpts：当前的PTS值
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VENC_GetCurPts(FH_UINT64 *Systemcurpts);

/**
* fh_enc_init
*@brief 编码器驱动初始化，供内部调用，用户无需显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 fh_enc_init();

/**
* fh_enc_close
*@brief 编码器驱动退出，供内部调用，用户无需显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 fh_enc_close();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /*__MPI_VO_H__ */

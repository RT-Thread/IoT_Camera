#ifndef __FH_VPU_MPI_H__
#define __FH_VPU_MPI_H__

#include "types/type_def.h"
#include "fh_vpu_mpipara.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
* FH_VPSS_SetViAttr
*@brief 设置视频输入属性
*@param [in] pstViconfig：视频输入属性值的指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SetViAttr(FH_VPU_SIZE *pstViconfig);

/**
* FH_VPSS_GetViAttr
*@brief 获取视频输入属性
*@param [in] 无
*@param [out] pstViconfig：视频输入属性值的指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetViAttr(FH_VPU_SIZE *pstViconfig);

/**
* FH_VPSS_SysInitMem
*@brief 初始化VPSS模块的系统内存，供内部调用，用户不需要显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SysInitMem(void);

/**
* FH_VPSS_Enable
*@brief 指定模式启用VPU 通道，如果更换模式时，需先调用FH_VPSS_Disable,然后再调用此函数
*@param [in] mode 选择VPU输入模式，从ISP获取数据或从MEM 获取数据
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_Enable(FH_VPU_VI_MODE mode);

/**
* FH_VPSS_Disable
*@brief 禁用VPSS。
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_Disable(void);

/**
* FH_VPSS_FreezeVideo
*@brief 视频冻结
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_FreezeVideo(void);

/**
* FH_VPSS_UnfreezeVideo
*@brief 视频解冻
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_UnfreezeVideo(void);

/**
* FH_VPSS_GetChnFrame
*@brief 从VPU通道获取一帧图像,阻塞
*@param [in] chan 通道号，取值0-3
*@param [out] pstVpuframeinfo 获取的图像信息指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetChnFrame(FH_UINT32 chan, FH_VPU_STREAM *pstVpuframeinfo);

/**
* FH_VPSS_SendUserPic
*@brief 支持用户发送图片信息进行编码，可用于视频丢失时的插入自定义图片
*@param [in] pstUserPic 用户发送到图片信息指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SendUserPic(const FH_VPU_USER_PIC *pstUserPic);

/**
* FH_VPSS_GetUserPicAddr
*@brief 获取用于存放用户图片的内存地址，可用于视频丢失时的插入自定义图片。
*@param [in] 无
*@param [out] pstUserPic 用户发送到图片信息指针。
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetUserPicAddr(FH_VPU_USER_PIC *pstUserPic);

// FH_SINT32 FH_VPSS_OpenModule(FH_UINT32 moduleenablebit);
// FH_SINT32 FH_VPSS_CloseModule(FH_UINT32 moduleenablebit);
// FH_SINT32 FH_VPSS_QueryModuleBit(FH_UINT32  *enablestatusbit);

/**
* FH_VPSS_SetChnAttr
*@brief 设置VPU 通道属性
*@param [in] chan 通道号，取值0-3
*@param [in] pstChnconfig VPU通道的属性指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SetChnAttr(FH_UINT32 chan, const FH_VPU_CHN_CONFIG *pstChnconfig);

/**
* FH_VPSS_GetChnAttr
*@brief 获取VPU 通道设置的属性值
*@param [in] chan 通道号，取值0-3
*@param [out] pstChnconfig VPU通道的属性指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetChnAttr(FH_UINT32 chan, FH_VPU_CHN_CONFIG *pstChnconfig);

FH_SINT32 FH_VPSS_SetVOMode(FH_UINT32 chan, FH_VPU_VO_MODE mode);

/**
* FH_VPSS_OpenChn
*@brief 打开VPU通道
*@param [in] chan 通道号，取值0-3
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_OpenChn(FH_UINT32 chan);

/**
* FH_VPSS_CloseChn
*@brief 关闭VPU 通道
*@param [in] chan 通道号，取值0-3
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_CloseChn(FH_UINT32 chan);

/**
* FH_VPSS_VOU_SetAttr
*@brief 设置VOU的数据源的通道属性
*@param [in] pstChnconfig VPU通道的属性指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_VOU_SetAttr(const FH_VPU_CHN_CONFIG *pstChnconfig);

/**
* FH_VPSS_VOU_GetAttr
*@brief 获取VOU的数据源的通道属性
*@param [in] 无
*@param [out] pstChnconfig VPU通道的属性指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_VOU_GetAttr(FH_VPU_CHN_CONFIG *pstChnconfig);

/**
* FH_VPSS_SetMask
*@brief 设置VPU 通道视频覆盖区域，通过设置参数Enable成员控制有效
*@param [in] pstVpumaskinfo  MASK的属性指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SetMask(const FH_VPU_MASK *pstVpumaskinfo);

/**
* FH_VPSS_GetMask
*@brief 获取VPU 通道视频覆盖区域
*@param [in] 无
*@param [out] pstVpumaskinfo  MASK的属性指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetMask(FH_VPU_MASK *pstVpumaskinfo);

/**
* FH_VPSS_ClearMask
*@brief 清除相应的视频覆盖区域
*@param [in] maskAreaIdx 视频覆盖区域号,取值0-7
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_ClearMask(FH_UINT32 clearmaskarea);

/**
* FH_VPSS_SetMask
*@brief 设置VPU通道logo叠加信息，通过设置参数Enable成员控制有效
*@param [in] pstVpugraphinfo logo叠加参数指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SetGraph(const FH_VPU_LOGO *pstVpugraphinfo);

/**
* FH_VPSS_GetGraph
*@brief 获取VPU通道logo叠加信息
*@param [in] 无
*@param [out] pstVpugraphinfo logo叠加参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetGraph(FH_VPU_LOGO *pstVpugraphinfo);

/**
* FH_VPSS_SetOsd
*@brief 设置VPU 通道字符叠加
*@param [in] pstVpuosdinfo：字符叠加参数指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SetOsd(const FH_VPU_OSD *pstVpuosdinfo);

/**
* FH_VPSS_GetOsd
*@brief 获取VPU 通道字符叠加信息
*@param [in] 无
*@param [out] pstVpuosdinfo：字符叠加参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetOsd(FH_VPU_OSD *pstVpuosdinfo);

/**
* FH_VPSS_SetRotate
*@brief VPU 通道字符叠加旋转
*@param [in] Rotate 旋转角度设置
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SetRotate(const FH_OSD_ROTATE Rotate);

/**
* FH_VPSS_SetOsdInvert
*@brief 设置VPU 通道字符叠加反色控制信息
*@param [in] pstOsdinvertctl 字符叠加的反色控制位指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SetOsdInvert(const FH_INVERT_CTRL *pstOsdinvertctl);

/**
* FH_VPSS_GetOsdInvert
*@brief 获取VPU 通道字符叠加反色控制信息
*@param [in] 无
*@param [out] pstOsdinvertctl 字符叠加的反色控制位指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetOsdInvert(FH_INVERT_CTRL *pstOsdinvertctl);

/**
* FH_VPSS_EnableYCmean
*@brief 开启YC均值统计值。
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_EnableYCmean(void);

/**
* FH_VPSS_DisableYCmean
*@brief 关闭YC均值统计值
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_DisableYCmean(void);

/**
* FH_VPSS_GetYCmean
*@brief 获取YC均值统计值
*@param [in] 无
*@param [out] pstVpuycmeaninfo：YC均值计算参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetYCmean(FH_VPU_YCMEAN *pstVpuycmeaninfo);

/**
* FH_VPSS_SetFramectrl
*@brief 设置帧率控制参数
*@param [in] chan 通道号，取值0-3
*@param [in]	pstFramerate：帧率控制参数指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SetFramectrl(FH_UINT32 chan, const FH_FRAMERATE *pstVpuframectrl);

/**
* FH_VPSS_GetFramectrl
*@brief 获取设置的帧率控制参数
*@param [in] chan 通道号，取值0-3
*@param [out] pstFramerate 帧率控制参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetFramectrl(FH_UINT32 chan, FH_FRAMERATE *pstVpuframectrl);

/**
* FH_VPSS_GetFrameRate
*@brief 获取当前通道统计得到的帧率
*@param [in] chan 通道号，取值0-3
*@param [out] pstFramerate 帧率
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetFrameRate(FH_UINT32 chan, FH_FRAMERATE *fps);

/**
* FH_VPSS_LOW_LATENCY_Enable
*@brief 开启低延时模式，同时只能有一个通道使能低延时模式
*@param [in] chan 通道号，取值0-2
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_LOW_LATENCY_Enable(FH_UINT32 chan);

/**
* FH_VPSS_LOW_LATENCY_Disable
*@brief 禁用低延时模式
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_LOW_LATENCY_Disable(void);

/**
* FH_VPSS_SetCrop
*@brief 设置VPU 通道裁剪功能参数
*@param [in] chan 通道号，取值0-3
*@param [in] pstVpucropinfo 通道裁剪功能参数指针
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_SetCrop(FH_UINT32 chan, const FH_VPU_CROP *pstVpucropinfo);

/**
* FH_VPSS_GetCrop
*@brief 获取VPU 通道裁剪功能属性
*@param [in] chan 通道号，取值0-3
*@param [out] pstVpucropinfo 通道裁剪功能参数指针
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_GetCrop(FH_UINT32 chan, FH_VPU_CROP *pstVpucropinfo);

/**
* FH_VPSS_Reset
*@brief VPU 模块reset,该函数仅置下复位的标志位，并不执行复位操作
*				，返回并不表示reset完成。
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPSS_Reset(void);

/**
* FH_VPU_GetPkginfo
*@brief 获取VPU PKG模式下寄存器的配置值
*@param [in] 无
*@param [out] pstVpupkginfo PKG模块下寄存器配置值
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 FH_VPU_GetPkginfo(FH_PKG_INFO *pstVpupkginfo);

/**
* 一些调试函数，用户无需调用
*/
FH_SINT32 FH_VPSS_ReadMallocedMem(int intMemSlot, FH_UINT32 offset, FH_UINT32 *pstData);
FH_SINT32 FH_VPSS_WriteMallocedMem(int intMemSlot, FH_UINT32 offset, FH_UINT32 *pstData);
FH_SINT32 FH_VPSS_ImportMallocedMem(int intMemSlot, FH_UINT32 offset, FH_UINT32 *pstSrc, FH_UINT32 size);
FH_SINT32 FH_VPSS_ExportMallocedMem(int intMemSlot, FH_UINT32 offset, FH_UINT32 *pstDst, FH_UINT32 size);

/**
* fh_vpu_init
*@brief VPU驱动初始化，供内部调用，用户无需显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 fh_vpu_init();

/**
* fh_vpu_close
*@brief VPU驱动初始化，供内部调用，用户无需显性调用
*@param [in] 无
*@param [out] 无
*@return 是否成功
* - RETURN_OK(0):  成功
* - 其他：失败,错误详见错误号
*/
FH_SINT32 fh_vpu_close();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /*__MPI_VO_H__ */

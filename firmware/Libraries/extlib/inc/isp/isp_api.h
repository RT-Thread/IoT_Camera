#ifndef _ISP_API_H_
#define _ISP_API_H_

#include "isp_common.h"
#include "isp_sensor_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define lift_shift_bit_num(bit_num) (1 << bit_num)

enum ISP_HW_MODULE_LIST
{
    HW_MODUL_CLP        = lift_shift_bit_num(0),
    HW_MODUL_GB         = lift_shift_bit_num(1),
    HW_MODUL_DPCINGB    = lift_shift_bit_num(2),
    HW_MODUL_GAIN       = lift_shift_bit_num(3),
    HW_MODUL_LSC        = lift_shift_bit_num(4),
    HW_MODUL_NR3D       = lift_shift_bit_num(5),
    HW_MODUL_DPC        = lift_shift_bit_num(6),
    HW_MODUL_NR2D       = lift_shift_bit_num(7),
    HW_MODUL_AWB        = lift_shift_bit_num(8),
    HW_MODUL_HLR        = lift_shift_bit_num(9),
    HW_MODUL_LTM        = lift_shift_bit_num(10),
    HW_MODUL_APC        = lift_shift_bit_num(11),
    HW_MODUL_CGAMMA     = lift_shift_bit_num(12),
    HW_MODUL_CE         = lift_shift_bit_num(13),
    HW_MODUL_YE         = lift_shift_bit_num(14),
    HW_MODUL_ECS        = lift_shift_bit_num(15),
    HW_MODUL_HCS        = lift_shift_bit_num(16),
    HW_MODUL_GLOBE_STAT = lift_shift_bit_num(17),
    HW_MODUL_HIST       = lift_shift_bit_num(18),
};

typedef struct mirror_cfg_s
{
    FH_BOOL bEN;
    ISP_BAYER_TYPE normal_bayer;
    ISP_BAYER_TYPE mirror_bayer;
} MIRROR_CFG_S;

FH_SINT32 API_ISP_MemInit(FH_UINT32 width, FH_UINT32 height);
// int API_ISP_GetMemSize(FH_UINT32 *u32Memsize);
// int API_ISP_MemCfg(FH_UINT32 u32MemAddr,FH_UINT32 u32MemSize);

/**
 * @brief		获取ISP的param参数的地址和大小
 *
 * @param[out]	*u32BinAddr: param的地址
 * @param[out]   *u32BinSize: param的size
 *
 * @return
 *
 */
FH_SINT32 API_ISP_GetBinAddr(FH_UINT32 *u32BinAddr, FH_UINT32 *u32BinSize);

FH_SINT32 API_ISP_SetViHwAttr(const ISP_VI_HW_ATTR *pstViHwAttr);

FH_SINT32 API_ISP_SensorInit(void);
FH_SINT32 API_ISP_SetSensorFmt(FH_UINT32 format);
FH_SINT32 API_ISP_SetSensorReg(FH_UINT16 addr, FH_UINT16 data);
FH_SINT32 API_ISP_SetCisClk(FH_UINT8 cisClk);
FH_SINT32 API_ISP_SetViAttr(const ISP_VI_ATTR_S *pstViAttr);
FH_SINT32 API_ISP_GetViAttr(ISP_VI_ATTR_S *pstViAttr);
FH_SINT32 API_ISP_Init(void);
FH_SINT32 API_ISP_SetIspParam(ISP_DEFAULT_PARAM *pstDefaultParam);
FH_SINT32 API_ISP_LoadIspParam(char *isp_param_buff);
FH_SINT32 API_ISP_Pause(FH_VOID);
FH_SINT32 API_ISP_KickStart(void);
FH_SINT32 API_ISP_Resume(FH_VOID);
FH_SINT32 API_ISP_DetectPicSize(void);
/*
 * @brief	ISP策略处理函数
 *
 * @return 	0, 处理正常
 * 			-1， 图像丢失
 * 			1， 图像恢复
 */
FH_SINT32 API_ISP_Run(FH_VOID);
FH_SINT32 API_ISP_Exit(FH_VOID);
FH_SINT32 API_ISP_SensorRegCb(FH_UINT32 u32SensorId, struct isp_sensor_if *pstSensorFunc);
FH_SINT32 API_ISP_SensorUnRegCb(FH_UINT32 u32SensorId);

FH_SINT32 API_ISP_Set_HWmodule_cfg(FH_UINT32 u32modulecfg);
FH_SINT32 API_ISP_Get_HWmodule_cfg(FH_UINT32 *u32modulecfg);

FH_SINT32 API_ISP_WaitVD(FH_VOID);
FH_SINT32 API_ISP_GetVersion(ISP_VERSION *pstIspVersion);
FH_SINT32 API_ISP_SetIspReg(FH_UINT32 u32RegAddr, FH_UINT32 u32RegData);
FH_SINT32 API_ISP_GetIspReg(FH_UINT32 u32RegAddr, FH_UINT32 *u32RegData);
FH_SINT32 API_ISP_GetRaw(FH_BOOL strategy_en, FH_VOID *pRawBuff, FH_UINT32 u32Size, FH_UINT32 u32FrameCnt);
FH_SINT32 API_ISP_CheckStatReady(FH_BOOL *isReady);
/**  AE */
FH_SINT32 API_ISP_AEAlgEn(FH_BOOL bEn);
FH_SINT32 API_ISP_AESendCmd(FH_UINT32 u32Cmd, FH_VOID *param);
FH_SINT32 API_ISP_GetAeStat(ISP_AE_STAT *pstAeStat);
FH_SINT32 API_ISP_SetAeInfo(ISP_AE_INFO *pstAeInfo);
FH_SINT32 API_ISP_GetAeInfo(ISP_AE_INFO *pstAeInfo);
FH_SINT32 API_ISP_GetAeCfg(AE_DEFAULT_CFG *pstAeDefaultCfg);
/**  AWB */
FH_SINT32 API_ISP_GetAwbStat(const ISP_AWB_INFO *pstAwbInfo);
FH_SINT32 API_ISP_SetAwbCfg(ISP_AWB_CFG *pstAwbCfg);
FH_SINT32 API_ISP_AWBAlgEn(FH_BOOL bEn);
FH_SINT32 API_ISP_AWBSendCmd(FH_UINT32 u32Cmd, FH_VOID *param);
FH_SINT32 API_ISP_GetAwbCfg(AWB_DEFAULT_CFG *pstAwbDefaultCfg);
/** GLOBESTATE **/
FH_SINT32 API_ISP_GetGlobeStat(GLOBE_STAT *pstGlobeStat);
FH_SINT32 API_ISP_SetGlobeStat(GLOBE_STAT_CFG *pstGlobeStat);
/**  AF*/
FH_SINT32 API_ISP_AFlibRegCb(FH_VOID);
FH_SINT32 API_ISP_AFlibUnRegCb(FH_VOID);
FH_SINT32 API_ISP_AFAlgEn(FH_BOOL bEn);
FH_SINT32 API_ISP_AFSendCmd(FH_UINT32 u32Dev, FH_UINT32 u32Cmd, FH_VOID *param);
FH_SINT32 API_ISP_SetAFFilter(const ISP_AF_FILTER *pstAfFilter);
FH_SINT32 API_ISP_SetAFWinInfo(const ISP_AF_WIN_INFO *pstAfWinInfo);
FH_SINT32 API_ISP_GetAFStat(ISP_AF_STAT *pstAfStat);
/**LTM*/
FH_SINT32 API_ISP_SetLtmCfg(const ISP_LTM_CFG *pstLtmCfg);
FH_SINT32 API_ISP_GetLtmCfg(ISP_LTM_CFG *pstLtmCfg);
/**  BLC function  */
FH_SINT32 API_ISP_SetBlcAttr(const ISP_BLC_ATTR *pstBlcAttr);
FH_SINT32 API_ISP_GetBlcAttr(ISP_BLC_ATTR *pstBlcAttr);
/** FFPN*/
FH_SINT32 API_ISP_SetStatFfpnCfg(const ISP_FFPN_CFG *pstFfpnCfg);
FH_SINT32 API_ISP_GetStatFfpnCfg(ISP_FFPN_CFG *pstFfpnCfg);
FH_SINT32 API_ISP_GetFfpnStat(ISP_FFPN_INFO *pstFfpnCfg);
FH_SINT32 API_ISP_SetFfpnCfg(const ISP_FFPN_CFG *pstFfpnCfg);
/** PFPN*/
FH_SINT32 API_ISP_SetPfpnCfg(const ISP_PFPN_CFG *pstPfpnCfg);
FH_SINT32 API_ISP_GetPfpnCfg(ISP_PFPN_CFG *pstPfpnCfg);
/** GB*/
FH_SINT32 API_ISP_SetGbCfg(const ISP_GB_CFG *pstGbCfg);
FH_SINT32 API_ISP_GetGbCfg(ISP_GB_CFG *pstGbCfg);
/** DPC*/
FH_SINT32 API_ISP_SetStaticDpc(const STATIC_DPC_CFG *pstStaticDpc);
FH_SINT32 API_ISP_GetStaticDpc(STATIC_DPC_CFG *pstStaticDpc);
FH_SINT32 API_ISP_SetDynamicDpc(const DYNAMIC_DPC_CFG *pstDynamicDpc);
FH_SINT32 API_ISP_GetDynamicDpc(DYNAMIC_DPC_CFG *pstDynamicDpc);
/** LSC*/
FH_SINT32 API_ISP_SetLscCfg(const ISP_LSC_CFG *pstLscCfg);
FH_SINT32 API_ISP_GetLscCfg(ISP_LSC_CFG *pstLscCfg);
/** NR3D*/
FH_SINT32 API_ISP_SetNR3D(const ISP_NR3D_CFG *pstNr3dCfg);
FH_SINT32 API_ISP_GetNR3D(ISP_NR3D_CFG *pstNr3dCfg);
FH_SINT32 API_ISP_SetNRWeightTab(const ISP_NR_TABLE *pstNr3dTab);
FH_SINT32 API_ISP_GetNRWeightTab(ISP_NR_TABLE *pstNr3dTab);
/** NR2D*/
FH_SINT32 API_ISP_SetNR2DCfg(const ISP_NR2D_CFG *pstNr2dCfg);
FH_SINT32 API_ISP_GetNR2DCfg(ISP_NR2D_CFG *pstNr2dCfg);

/**HLR*/
FH_SINT32 API_ISP_SetHlrCfg(const ISP_HLR_CFG *pstHlrCfg);
FH_SINT32 API_ISP_GetHlrCfg(ISP_HLR_CFG *pstHlrCfg);

/**post gain*/
int API_ISP_SetPostGain(const ISP_POST_GAIN_CFG *pstPostGainCfg);
int API_ISP_GetPostGain(ISP_POST_GAIN_CFG *pstPostGainCfg);

/**Contrast*/
FH_SINT32 API_ISP_SetContrastCfg(const ISP_CONTRAST_CFG *pstContrastCfg);
FH_SINT32 API_ISP_GetContrastCfg(ISP_CONTRAST_CFG *pstContrastCfg);
FH_SINT32 API_ISP_SetBrightnessCfg(ISP_BRIGHTNESS_CFG *pstBrightnessCfg);
FH_SINT32 API_ISP_GetBrightnessCfg(ISP_BRIGHTNESS_CFG *pstBrightnessCfg);
/**CE*/
FH_SINT32 API_ISP_SetSaturation(const ISP_SAT_CFG *pstCeCfg);
FH_SINT32 API_ISP_GetSaturation(ISP_SAT_CFG *pstCeCfg);
/**APC*/
FH_SINT32 API_ISP_SetApcCfg(const ISP_APC_CFG *pstApcCfg);
FH_SINT32 API_ISP_GetApcCfg(ISP_APC_CFG *pstApcCfg);
/**GAMMA*/
FH_SINT32 API_ISP_SetGammaCfg(const ISP_GAMMA_CFG *pstGammaCfg);
FH_SINT32 API_ISP_GetGammaCfg(ISP_GAMMA_CFG *pstGammaCfg);
/**YHIST*/
FH_SINT32 API_ISP_GetYhist(ISP_YHIST_STAT *pstYhistStat);
/**RGBA*/
FH_SINT32 API_ISP_RgbaEn(FH_BOOL bEn);
FH_SINT32 API_ISP_RgbaSendCmd(unsigned int u32Cmd, void *param);
FH_SINT32 API_ISP_GetRgbaCfg(RGBA_DEFAULT_CFG *pstRgbaDefaultCfg);
FH_SINT32 API_ISP_GetRgbaStat(RGBA_STAT_INFO *pstRgbaStat);
FH_SINT32 API_ISP_GetRgbaInfo(RGBA_CURR_INFO *pstRgbaInfo);

FH_SINT32 API_ISP_SetIrStatus(FH_UINT32 status);
FH_SINT32 API_ISP_GetIrStatus(FH_UINT32 *status);

/**VPU NR*/
FH_SINT32 API_ISP_SetYnrCfg(const ISP_YNR_CFG *pstYnrCfg);
FH_SINT32 API_ISP_GetYnrCfg(ISP_YNR_CFG *pstYnrCfg);
FH_SINT32 API_ISP_SetCnrCfg(const ISP_CNR_CFG *pstCnrCfg);
FH_SINT32 API_ISP_GetCnrCfg(ISP_CNR_CFG *pstYnrCfg);

/**VPU APC*/
FH_SINT32 API_ISP_SetAntiPurpleBoundary(FH_VOID *param);
FH_SINT32 API_ISP_GetAntiPurpleBoundary(FH_VOID *param);

/**Debug Interface**/
FH_SINT32 API_ISP_ReadMallocedMem(FH_SINT32 intMemSlot, FH_UINT32 offset, FH_UINT32 *pstData);
FH_SINT32 API_ISP_WriteMallocedMem(FH_SINT32 intMemSlot, FH_UINT32 offset, FH_UINT32 *pstData);
FH_SINT32 API_ISP_ImportMallocedMem(FH_SINT32 intMemSlot, FH_UINT32 offset, FH_UINT32 *pstSrc, FH_UINT32 size);
FH_SINT32 API_ISP_ExportMallocedMem(FH_SINT32 intMemSlot, FH_UINT32 offset, FH_UINT32 *pstDst, FH_UINT32 size);

FH_SINT32 API_ISP_MirrorEnable(MIRROR_CFG_S *pMirror);
FH_SINT32 API_ISP_SetMirrorAndflip(FH_BOOL mirror, FH_BOOL flip);
FH_SINT32 API_ISP_SetMirrorAndflipEx(FH_BOOL mirror, FH_BOOL flip, FH_UINT32 bayer);
FH_SINT32 API_ISP_GetMirrorAndflip(FH_BOOL *mirror, FH_BOOL *flip);

FH_SINT32 API_ISP_GetVIState(ISP_VI_STAT_S *pstStat);
FH_SINT32 API_ISP_SetSensorFrameRate(int m);
FH_SINT32 API_ISP_Dump_Param(FH_UINT32 *addr, FH_UINT32 *size);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*_ISP_API_H_*/

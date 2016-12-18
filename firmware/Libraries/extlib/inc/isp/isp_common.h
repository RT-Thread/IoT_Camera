#ifndef _ISP_COMMON_H_
#define _ISP_COMMON_H_

#include "types/type_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define MALLOCED_MEM_BASE_ISP (0)
#define MALLOCED_MEM_IPFPKG (0)
#define MALLOCED_MEM_IPFBPKG (1)
#define MALLOCED_MEM_IPBPKG (2)
#define MALLOCED_MEM_DRV (3)

#define GAIN_NODES 12
#define LINEAR_INTERPOLATION(gain0, gain1, y0, y1, gainx) \
    (((gainx) - (gain0)) * (y1) + ((gain1) - (gainx)) * (y0)) / ((gain1) - (gain0))

typedef enum _ISP_BAYER_TYPE_S_ {
    BAYER_RGGB = 0x0,
    BAYER_GRBG = 0x1,
    BAYER_BGGR = 0x2,
    BAYER_GBRG = 0x3,  //

    BAYER_BGAR = 4 + 0x0,
    BAYER_RGAB = 4 + 0x1,
    BAYER_GBRA = 4 + 0x2,
    BAYER_GRBA = 4 + 0x3,
    BAYER_BAGR = 4 + 0x4,
    BAYER_RAGB = 4 + 0x5,
    BAYER_ABRG = 4 + 0x6,
    BAYER_ARBG = 4 + 0x7,

} ISP_BAYER_TYPE;

typedef enum _CURVE_TYPE_ { CURVE_BUILTIN = 0x1, CURVE_USER_DEFINE = 0x2 } CURVE_TYPE;

typedef enum _SNS_CLK_S_ {
    SNS_CLK_24_POINT_0   = 0x1,
    SNS_CLK_27_POINT_0   = 0x2,
    SNS_CLK_37_POINT_125 = 0x3,
} SNS_CLK;

typedef enum _SNS_DATA_BIT_S_ {
    LINER_DATA_8_BITS  = 0x1,
    LINER_DATA_12_BITS = 0x2,
    LINER_DATA_14_BITS = 0x3,
    WDR_DATA_16_BITS   = 0x4,
} SNS_DATA_BITS;

typedef enum _SIGNAL_POLARITY_S_ { ACTIVE_HIGH = 0x0, ACTIVE_LOW = 0x1 } SIGNAL_POLARITY;

typedef enum _AE_CMD_KEY_S_ {
    SET_AE_DEFAULT = 0x0,
    SET_ASC_EN,
    SET_AGC_EN,
    SET_APERTURE_EN,
    SET_LUMA_REF,
    SET_STAB_ZONE,
    SET_STAB_CNT,
    SET_DGAIN_MAX,
    SET_SENSOR_GAIN_MAX,
    SET_INTT_MAX,
    SET_INTT_FINE_TIMING,
    SET_GAIN_PREC,
    SET_INTT_PREC,
    SET_INTT_MIN,
    SET_AE_SPPED,
    SET_METERING_MODE,
    SET_SENSOR_UP_MAX,
    SET_SENSOR_UPEN_GAIN,
    SET_SENSOR_UPDIS_GAIN,
    SET_ADAPTIVE_EN,
    SET_LUMA_REF_LOW,
} AE_CMD_KEY;

typedef enum _AWB_CMD_KEY_S_ {
    SET_AWB_DEFAULT = 0x0,
    CTRL_AWB_ENABLE,
    SET_AWB_SPEED,
    SET_AWB_WHITEPOINT,
    SET_STAT_CTRL,
    SET_CCM_ARRAY,
    SET_AWB_GAIN,
    GET_AWB_GAIN,
} AWB_CMD_KEY;

typedef struct _ISP_VERSION_S_
{
    FH_UINT32 u32SdkVer;
    FH_UINT32 FH_UINT32ChipVer;
    FH_UINT8 u08SdkSubVer;
    FH_UINT8 u08BuildTime[21];
} ISP_VERSION;

typedef struct _ISP_VI_HW_ATTR_S_
{
    SNS_CLK eSnsClock;
    SNS_DATA_BITS eSnsDataBits;
    SIGNAL_POLARITY eHorizontalSignal;
    SIGNAL_POLARITY eVerticalSignal;
    FH_BOOL u08Mode16;
    FH_UINT32 u32DataMaskHigh;
    FH_UINT32 u32DataMaskLow;
} ISP_VI_HW_ATTR;

typedef struct _ISP_VI_ATTR_
{
    FH_UINT16 u16WndHeight;          ///<sensor幅面高度
    FH_UINT16 u16WndWidth;           /// <sensor幅面宽度
    FH_UINT16 u16InputHeight;        ///<sensor输入图像高度
    FH_UINT16 u16InputWidth;         ///<sensor输入图像宽度
    FH_UINT16 u16OffsetX;            ///<裁剪水平偏移
    FH_UINT16 u16OffsetY;            ///<裁剪垂直偏移
    FH_UINT16 u16PicHeight;          ///<处理的图像高度
    FH_UINT16 u16PicWidth;           ///<处理的图像宽度
    FH_UINT16 u16FrameRate;          ///<帧率
    ISP_BAYER_TYPE enBayerType;      ///<bayer数据格式
    ISP_BAYER_TYPE enRgbaBayerType;  ///<rgba bayer数据格式
} ISP_VI_ATTR_S;

typedef struct _ISP_BLC_ATTR_S_
{
    FH_BOOL bGainmappingEn;
    FH_UINT16 u16Blclv;
    FH_UINT16 u16Blc[GAIN_NODES];
} ISP_BLC_ATTR;

typedef struct _ISP_FFPN_CFG_S_
{
    FH_BOOL bFfpnEn;           ///<是否进入FPN统计。
    FH_UINT16 u16FpnStatGain;  ///<FPN统计时的sensor gain。
    FH_UINT16 u16Numexp;       ///<2^(num_exp+1)==列统计点数
    FH_UINT16 u16Thl;          ///<饱和度像素阈值
    FH_UINT16 u16Thh;          ///<饱和度像素阈值
} ISP_FFPN_CFG;

typedef struct _ISP_FFPN_INFO_S_
{
    FH_UINT32 u16StatFfpn[1280];
    FH_UINT16 u16FpnData[1280];
} ISP_FFPN_INFO;

typedef struct _ISP_PFPN_CFG_S_
{
    FH_BOOL bPfpnEn;
    FH_UINT8 u08PfpnMode;
    FH_UINT8 u08DpEn;
    FH_UINT16 u16PfpnGain;
} ISP_PFPN_CFG;

typedef struct _ISP_GB_CFG_S_
{
    FH_BOOL bGBEn;
    FH_UINT16 u16Th;
    FH_UINT16 u16Tl;
} ISP_GB_CFG;

typedef struct _DYNAMIC_DPC_CFG_S_
{
    FH_BOOL bDpcEn;
    FH_BOOL bGainMappingEn;
    FH_UINT8 ctrl_mode;
    FH_UINT8 u08Str;
    FH_UINT8 u08DpcMode;               // 0:all close;1:white pixel correct open;2.black pixel correct open;3.all open.
    FH_UINT8 u08Strenght[GAIN_NODES];  // 包括:mode,w_s,b_s
    FH_UINT8 u08WhiteThr[GAIN_NODES];  //白点门限DC值
    FH_UINT8 u08BlackThr[GAIN_NODES];  //黑点门限值
} DYNAMIC_DPC_CFG;

typedef struct _STATIC_DPC_CFG_S_
{
    FH_BOOL bStaticDccEn;
    FH_BOOL bStaticDpcEn;
    FH_UINT32 u32DpcTable[1024];
    FH_UINT16 u16DpcCol[32];
} STATIC_DPC_CFG;

typedef struct _ISP_LSC_CFG_S_
{
    FH_BOOL bLscEn;
    FH_UINT32 u32Coff[299];
} ISP_LSC_CFG;

typedef struct _ISP_NR3D_CFG_S_
{
    FH_BOOL bNR3DEn;
    FH_BOOL bGainMappingEn;
    FH_UINT8 u08Str;
    FH_UINT16 u16K1;
    FH_UINT16 u16K2;
    FH_UINT16 u16O1;
    FH_UINT16 u16O2;
    FH_UINT8 u08StrMap[GAIN_NODES];
} ISP_NR3D_CFG;

typedef struct _ISP_NR_TABLE_S_
{
    FH_UINT32 u32NRWeight[8];
} ISP_NR_TABLE;

typedef struct _ISP_LTM_CFG_S_
{
    FH_BOOL bLtmEn;
    CURVE_TYPE eCurveType;
    FH_UINT8 u32LtmCurveIdx;
    FH_UINT32 u32TonemapCurve[128];
    FH_UINT32 u32LtmIdx;
} ISP_LTM_CFG;

typedef struct _ISP_NR2D_CFG_S_
{
    FH_BOOL bNR2DEn;
    FH_BOOL bGainMappingEn;
    FH_UINT8 u08Str;
    FH_UINT16 u16K1;
    FH_UINT16 u16K2;
    FH_UINT16 u16O1;
    FH_UINT16 u16O2;
    FH_UINT8 u08StrMap[GAIN_NODES];
} ISP_NR2D_CFG;

typedef struct _ISP_HLR_CFG_S_
{
    FH_BOOL bHlrEn;
    FH_BOOL bAuto;
    FH_UINT16 u16HlrMax;
} ISP_HLR_CFG;

typedef struct _ISP_POST_GAIN_CFG_S
{
    FH_UINT16 rGain;
    FH_UINT16 gGain;
    FH_UINT16 bGain;
} ISP_POST_GAIN_CFG;

typedef struct _ISP_CONTRAST_CFG_S_
{
    FH_BOOL bYcEn;
    FH_BOOL bGainMappingEn;
    FH_UINT8 u08Crt;
    FH_UINT8 u08Mid;
    FH_UINT8 u08CrtMap[GAIN_NODES];  //
} ISP_CONTRAST_CFG;

typedef struct _ISP_BRIGHTNESS_CFG_S_
{
    FH_BOOL bYcEn;
    FH_BOOL bGainMappingEn;
    FH_UINT8 u08Brt;
    FH_UINT8 u08BrtMap[GAIN_NODES];
} ISP_BRIGHTNESS_CFG;

typedef struct _ISP_SAT_CFG_S_
{
    FH_BOOL bGainMappingEn;
    FH_UINT8 u08Sat;
    FH_UINT8 u08BlueSurp;
    FH_UINT8 u08RedSurp;
    FH_UINT8 u08SatMap[GAIN_NODES];
} ISP_SAT_CFG;

typedef struct _ISP_APC_CFG_S_
{
    FH_BOOL bApcEn;
    FH_BOOL bGainMappingEn;
    FH_UINT8 u08EdgeLv;
    FH_UINT8 u08Detaillv;
    FH_UINT8 u08PositiveGain;
    FH_UINT8 u08NegativeGain;
    FH_UINT16 u16LowTh;
    FH_UINT16 u16HighTh;
    FH_UINT8 u08EdgeStr[GAIN_NODES];      // DEPGain、DENGain
    FH_UINT8 u08DetailStr[GAIN_NODES];    // ESPGain、ESNGain
    FH_UINT8 u08PositiveStr[GAIN_NODES];  // APCPGain
    FH_UINT8 u08NegativeStr[GAIN_NODES];  // APCNGain
    FH_UINT16 u16LowThr[GAIN_NODES];      // HAPCSTHL
    FH_UINT16 u16HighThr[GAIN_NODES];     // HAPCSTHH
} ISP_APC_CFG;

typedef enum _GAMMA_BUILTIN_IDX_ {
    GAMMA_CURVE_10   = 0,
    GAMMA_CURVE_12   = 1,
    GAMMA_CURVE_14   = 2,
    GAMMA_CURVE_16   = 3,
    GAMMA_CURVE_18   = 4,
    GAMMA_CURVE_20   = 5,
    GAMMA_CURVE_22   = 6,
    GAMMA_CURVE_24   = 7,
    GAMMA_CURVE_26   = 8,
    GAMMA_CURVE_28   = 9,
    GAMMA_CURVE_USER = 15,
} GAMMA_BUILTIN_IDX;

typedef struct _ISP_GAMMA_CFG_S_
{
    FH_BOOL bGammaEn;
    CURVE_TYPE eCurveType;
    GAMMA_BUILTIN_IDX eGammaBuiltInIdx;
    FH_UINT16 u16CGamma[82];
} ISP_GAMMA_CFG;

typedef struct _ISP_YHIST_STAT_S_
{
    FH_UINT32 u32YhistStatCnt[64];
} ISP_YHIST_STAT;

typedef struct _ISP_YNR_CFG_S_
{
    FH_BOOL bYnrEn;
    FH_BOOL bGainMappingEn;
    FH_UINT8 u8YnrThrm;
    FH_UINT8 u8YnrThr[GAIN_NODES];
} ISP_YNR_CFG;

typedef struct _ISP_CNR_CFG_S_
{
    FH_BOOL bCnrEn;
    FH_BOOL bGainMappingEn;
    FH_UINT8 u8CnrStrm;
    FH_UINT8 u8CnrEdgeThrm;
    FH_UINT8 u8CnrFlatThrm;
    FH_UINT8 u8CnrEdgeThr[GAIN_NODES];  // CNREdgeT
    FH_UINT8 u8CnrFlatThr[GAIN_NODES];  // CNREdgeT1
    FH_UINT8 u8CnrStr[GAIN_NODES];      // CNRSigma
} ISP_CNR_CFG;

typedef struct _ISP_AE_PARAM_S_
{
    FH_UINT32 u32Dev;
} ISP_AE_PARAM;

typedef struct _ISP_AE_STAT1_S_
{
    FH_UINT32 u32SumLuma[9];
    FH_UINT32 u32Cnt[9];
} ISP_AE_STAT1;

typedef struct _ISP_AE_STAT2_S_
{
    FH_UINT32 u32SumLuma[33];
    FH_UINT32 u32Cnt[33];
} ISP_AE_STAT2;

typedef struct _ISP_AE_STAT3_S_
{
    FH_UINT16 u32GlobalAvgLuma[1024];
} ISP_AE_STAT3;

typedef struct _ISP_AE_STAT_S_
{
    ISP_AE_STAT1 stAeStat1;
    ISP_AE_STAT2 stAeStat2;
} ISP_AE_STAT;

typedef struct _AE_STAT_CFG_S_
{
    FH_BOOL bChange;
    FH_UINT16 FH_UINT16HistStatThr[32];
} AE_STAT_CFG;

typedef struct _ISP_AE_INFO_S_
{
    FH_UINT32 u32Intt;
    FH_UINT32 u32IspGain;
    FH_UINT32 u32IspGainShift;
    FH_UINT32 u32SensorGain;
    FH_UINT32 u32TotalGain;
    AE_STAT_CFG stAeStatCfg;
} ISP_AE_INFO;

typedef struct _AE_DEFAULT_CFG_S_
{
    FH_BOOL bAscEn;
    FH_BOOL bAgcEn;
    FH_BOOL bApertureEn;
    FH_UINT8 u08LumaRef;
    FH_UINT8 u08StabZone0;
    FH_UINT8 u08StabZone1;
    FH_UINT8 u08StabCnt;
    FH_UINT16 u16DgainMax;
    FH_UINT16 u16SensorGainMax;
    FH_UINT16 u16InttMax;
    FH_UINT8 u08InttFineTiming;
    FH_UINT8 u08InttPrec;
    FH_UINT8 u08InttMin;
    FH_UINT8 u08AeSpeed;
    FH_UINT8 u08MeteringMode;
    FH_UINT8 u08MeteringParam;
    FH_UINT8 u08UpMax;
    FH_UINT16 u16UpEnGain;
    FH_UINT16 u16UpDisGAIN;
    FH_BOOL bAdaptiveEn;
    FH_UINT8 u08LumaRefLow;
    FH_UINT8 u08GainPrec;
} AE_DEFAULT_CFG;

typedef struct _ISP_AWB_PARAM_S_
{
    FH_UINT32 u32Dev;
} ISP_AWB_PARAM;

typedef struct _ISP_AWB_STAT1_S_
{
    FH_UINT32 u32AwbBlockCnt[9];
    FH_UINT32 u32AwbBlockB[9];
    FH_UINT32 u32AwbBlockG[9];
    FH_UINT32 u32AwbBlockR[9];
} ISP_AWB_STAT1;

typedef struct _ISP_AWB_STAT2_S_
{
    FH_UINT16 u32AwbBlockCnt[1024];
    FH_UINT16 u32AwbBlockB[1024];
    FH_UINT16 u32AwbBlockG[1024];
    FH_UINT16 u32AwbBlockR[1024];
} ISP_AWB_STAT2;

typedef struct _ISP_AWB_INFO_S_
{
    ISP_AWB_STAT1 stAwbStat1;
} ISP_AWB_INFO;

typedef struct _STAT_WHITE_POINT_S_
{
    FH_UINT16 u16Coordinate_w;
    FH_UINT16 u16Coordinate_h;
} STAT_WHITE_POINT;

typedef struct _AWB_STAT_CFG_S_
{
    FH_BOOL bChange;
    FH_UINT16 u16YHighThreshold;
    FH_UINT16 u16YLowThreshold;
    STAT_WHITE_POINT stPoint[7];
} AWB_STAT_CFG;

typedef struct _ISP_AWB_CFG_S_
{
    FH_UINT32 u32AwbGain[4];
    FH_UINT16 u16CcmCfg[9];
    AWB_STAT_CFG stAwbStatCtrl;
} ISP_AWB_CFG;

typedef struct _AWB_WHITE_POINT_S_
{
    FH_UINT16 u16BOverG[4];
    FH_UINT16 u16ROverG[4];
} AWB_WHITE_POINT;

typedef struct _AWB_WHITE_EREA_S_
{
    FH_UINT32 u32Area_P[5];
} AWB_WHITE_EREA;

typedef struct _CCM_TABLE_S_
{
    FH_UINT16 u16CcmTable[4][12];
} CCM_TABLE;

typedef struct _STAT_CTRL_S_
{
    FH_UINT8 u08WpLow;
    FH_UINT8 u08WpHigh;
    FH_UINT8 u08AwbThresholdL;
    FH_UINT8 U08AwbThresholdH;
} STAT_CTRL;

typedef struct _AWB_DEFAULT_CFG_S_
{
    FH_BOOL bAwbEn;
    FH_UINT16 u16AwbSpeed;
    AWB_WHITE_POINT stWhitePoint;
    AWB_WHITE_EREA stWhiteArea;
    STAT_CTRL stStatCtrl;
    CCM_TABLE stCcmTable;
} AWB_DEFAULT_CFG;

typedef struct _AWB_SENSOR_REG_S_
{
    FH_SINT32 (*pfAwbGetDefaultParam)(AWB_DEFAULT_CFG *pstIspAwbCfg);
} AWB_SENSOR_REG;

typedef struct _ISP_ALGORITHM_S_
{
    FH_UINT8 u08Name[16];
    FH_UINT8 u08AlgorithmId;
    FH_VOID (*run)(FH_VOID);
} ISP_ALGORITHM;

typedef struct _ISP_DEFAULT_PARAM_
{
    ISP_BLC_ATTR stBlcCfg;
    ISP_LTM_CFG stLtmCfg;
    ISP_GAMMA_CFG stGamma;
    ISP_SAT_CFG stSaturation;
    ISP_APC_CFG stApc;
    ISP_CONTRAST_CFG stContrast;
    ISP_BRIGHTNESS_CFG stBrt;
    ISP_NR3D_CFG stNr3d;
    ISP_NR_TABLE stNrTable;
    ISP_NR2D_CFG stNr2d;
    ISP_YNR_CFG stYnr;
    ISP_CNR_CFG stCnr;
    DYNAMIC_DPC_CFG stDpc;
    ISP_LSC_CFG stLscCfg;
} ISP_DEFAULT_PARAM;

typedef struct _ISP_VI_STAT_S
{
    FH_UINT32 u32IPBIntCnt; /* The video frame back interrupt count */
    FH_UINT32 u32IPFIntCnt; /* The video frame front interrupt count */
    FH_UINT32 u32FrmRate;   /* current frame rate */
    FH_UINT32 u32PicWidth;  /* curren pic width */
    FH_UINT32 u32PicHeight; /* current pic height */
} ISP_VI_STAT_S;

typedef struct _GLOBE_STAT_S
{
    struct _Block_gstat
    {
        FH_UINT32 sum;
        FH_UINT32 cnt;
        FH_UINT32 max;
        FH_UINT32 min;
    } r, gr, gb, b;
} GLOBE_STAT;

typedef struct _GLOBE_STAT_CFG_S
{
    FH_UINT8 width;
    FH_UINT8 height;
    FH_UINT8 cnt_x;
    FH_UINT8 cnt_y;
} GLOBE_STAT_CFG;

//----------------AF-------------------
typedef struct _ISP_AF_FILTER_S
{
    FH_UINT16 afFilterParam[11];
    FH_UINT8 afShift0;       //	u2
    FH_UINT8 afShift1;       //  u2
    FH_UINT8 afClipOrShift;  // 1:clip	0:shift
} ISP_AF_FILTER;

typedef struct _ISP_AF_WIN_INFO_S
{
    FH_UINT16 winHStart; /*the start pixel in horizontal direction, more than 2*/
    FH_UINT16 winVStart; /*the start pixel in vertical direction, more than 2*/
    FH_UINT8 winHCnt;    /*the number of window in horizontal direction*/
    FH_UINT8 winVCnt;    /*the number of window in vertical direction*/
    FH_UINT8 winWidth;   /*window width*/
    FH_UINT8 winHeight;  /*window height*/
} ISP_AF_WIN_INFO;

typedef struct _ISP_AF_STAT_ADDR_S
{
    FH_UINT32 startAddrH; /*the start address of stat in horizontal direction*/
    FH_UINT32 startAddrV; /*the start address of stat in vertical direction*/
} ISP_AF_STAT_ADDR;

typedef struct _ISP_AF_STAT_PIXEL_S
{
    FH_UINT32 max;
    FH_UINT32 sum;
} ISP_AF_STAT_PIXEL;

typedef struct _ISP_AF_STAT_S
{
    ISP_AF_STAT_PIXEL *statH;
    ISP_AF_STAT_PIXEL *statV;
} ISP_AF_STAT;

typedef struct _ISP_GME_PARAM_S_
{
    FH_SINT32 s32x;  // x vector
    FH_SINT32 s32y;  // y vector
} ISP_GME_PARAM;

//---------------------------RGBA----------------------------------------------
//-----------------------------------------------------------------------------
typedef struct _RGBA_FILTER_CFG_S_
{
    FH_UINT8 L_sel;   // U2
    FH_UINT8 C1_sel;  // U2
    FH_UINT8 C2_sel;  // U2
    FH_UINT8 C3_sel;  // U2
    FH_UINT8 L_ll_sel;
    FH_UINT8 C1_ll_sel;
    FH_UINT8 C2_ll_sel;
    FH_UINT8 C3_ll_sel;
} RGBA_FILTER_CFG;

typedef struct _RGBA_ABAL_CFG_S_
{
    FH_UINT8 abalSpeed;           // U4:  3
    FH_UINT8 abalThl;             // U4:	2
    FH_UINT8 abalThh;             // U4:	8
    FH_UINT8 abalStabTime;        // U8:	60
    FH_UINT8 abalStep;            // U4:	1
    FH_UINT8 abalTargetStabTime;  // U8:	60
    FH_UINT8 abalTargetThr;       // U4:	3
} RGBA_ABAL_CFG;

typedef struct _RGBA_WEIGHT_CFG_S_
{
    FH_UINT8 W2Speed;     // U4: 6
    FH_UINT8 W2Thl;       // U4:	2
    FH_UINT8 W2Thh;       // U4:	6
    FH_UINT8 W2StabTime;  // U8:	60
    FH_UINT8 W2Step;      // U4: 5
    FH_UINT8 W2AThl;      // U7
    FH_UINT8 W2AThh;      // U7
    FH_UINT16 W2GThl;     // U9
    FH_UINT16 W2GThh;     // U9
} RGBA_WEIGHT_CFG;

typedef struct _RGBA_CCM_TABLE_S_
{
    FH_UINT16 ccmTable[2][16];
} RGBA_CCM_TABLE;

typedef struct _RGBA_CONVERT_TH_S_
{
    FH_UINT16 convThl;
    FH_UINT16 convThh;
} RGBA_CONVERT_TH;

typedef struct _RGBA_ENHANCE_TH_S_
{
    FH_UINT16 enhThl;
    FH_UINT16 enhThh;
} RGBA_ENHANCE_TH;

typedef struct _RGBA_STAT_TH_S_
{
    FH_UINT16 statThl;
    FH_UINT16 statThh;
} RGBA_STAT_TH;

typedef struct _RGBA_EGAIN_CFG_S_
{
    FH_UINT8 egainR;
    FH_UINT8 egainG;
    FH_UINT8 egainB;
} RGBA_EGAIN_CFG;

typedef struct _RGBA_DEFAULT_CFG_S_
{
    FH_BOOL bRgbaEn;
    RGBA_FILTER_CFG filterParam;
    RGBA_ABAL_CFG abalParam;
    RGBA_WEIGHT_CFG weightParam;
    RGBA_CCM_TABLE ccmTab;
    RGBA_CONVERT_TH convTh;
    RGBA_ENHANCE_TH enhTh;
    RGBA_STAT_TH statTh;
    RGBA_EGAIN_CFG egain;
} RGBA_DEFAULT_CFG;

typedef struct _RGBA_STAT_INFO_S_
{
    struct rgba_blk_info
    {
        FH_UINT32 R_sum;
        FH_UINT32 G_sum;
        FH_UINT32 B_sum;
        FH_UINT32 A_sum;
        FH_UINT32 cnt;
        FH_UINT32 IRa_cnt;
        FH_UINT32 IRb_cnt;
    } blk_rgba[32];
} RGBA_STAT_INFO;

typedef struct _RGBA_CURR_INFO_S_
{
    FH_UINT16 abalTarget;
    FH_UINT16 W2Target;
    FH_UINT16 abalGainPre;
    FH_UINT8 abalCnt;
    FH_UINT8 abalTargetCnt;
    FH_UINT16 W2Pre;
    FH_UINT16 W2Cnt;
} RGBA_CURR_INFO;

typedef enum _RGBA_CMD_KEY_S_ {
    SET_RGBA_DEFAULT = 0x0,
    SET_RGBA_EN,
    SET_RGBA_FILTER,
    SET_ABAL_PARA,
    SET_WEIGHT_PARA,
    SET_CCM_TABLE,
    SET_CONVERT_TH,
    SET_ENHANCE_TH,
    SET_STAT_TH,
    SET_EGAIN,
} RGBA_CMD_KEY;
//-----------------------------------------------------------------------
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /*_ISP_COMMON_H_*/

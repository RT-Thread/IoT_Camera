#ifndef __FH_VENC_MPIPARA_H__
#define __FH_VENC_MPIPARA_H__

#include "types/type_def.h"
#include "fh_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct
{
    FH_ADDR lumma_addr;
    FH_ADDR chroma_addr;
    FH_UINT64 time_stamp;
    FH_SIZE size;
} FH_ENC_FRAME;

typedef enum {
    FH_PRO_BASELINE = 66, /**< baseline  */
    FH_PRO_MAIN     = 77, /**< main profile */
} FH_ENC_PROFILE_IDC;

typedef struct
{
    FH_BOOL chn_en;
    FH_ENC_PROFILE_IDC profile;
    FH_UINT32 i_frame_intterval;
    FH_SIZE size;
} FH_ENC_CHN_ATTR;

typedef enum {
    FH_RC_VBR    = 0,
    FH_RC_CBR    = 1,
    FH_RC_FIXEQP = 2,
} FH_ENC_RC_MODE;

typedef enum {
    FH_RC_VERYLOW = 0,
    FH_RC_LOW     = 1,
    FH_RC_MIDDLE  = 2,
    FH_RC_HIGH    = 3,
} FH_ENC_RC_LEVEL;

typedef struct
{
    FH_ENC_RC_MODE RCmode;
    FH_ENC_RC_LEVEL RClevel;
    FH_UINT32 bitrate;
    FH_UINT32 IminQP;
    FH_UINT32 ImaxQP;
    FH_UINT32 PminQP;
    FH_UINT32 PmaxQP;
    FH_UINT32 max_delay;     //(1-60)
    FH_FRAMERATE FrameRate;  //输出帧率
} FH_RC_CONFIG;

typedef struct
{
    unsigned int I_proportion;
    unsigned int P_proportion;
} FH_RC_I_DIV_P;

typedef struct
{
    FH_UINT32 init_qp;
    FH_ENC_CHN_ATTR chn_attr;
    FH_RC_CONFIG rc_config;
} FH_CHR_CONFIG;

typedef struct
{
    FH_UINT32 totalfrmcnt;    /**< 总共完成编码帧数*/
    FH_UINT32 totalstreamcnt; /**< 所有输出队列中待获取的帧数*/
} FH_SYS_STATUS;

typedef struct
{
    FH_UINT32 lastqp;
    FH_UINT32 bps;
    FH_UINT32 FrameToEnc;  //待编码帧数
    FH_UINT32 framecnt;    //已编码帧数
    FH_UINT32 streamcnt;   //输出队列中的帧数
} FH_CHN_STATUS;

typedef enum {
    NALU_P_SLICE = 0,
    NALU_I_SLICE = 2,
    NALU_IDR     = 5,
    NALU_SEI     = 6,
    NALU_SPS     = 7,
    NALU_PPS     = 8,
    NALU_AUD     = 9,
} FH_ENC_NALU_TYPE;

typedef struct
{
    FH_ENC_NALU_TYPE type;
    FH_UINT32 length;
    FH_UINT8 *start;
} FH_ENC_STREAM_NALU;

typedef enum {
    P_SLICE = 0,
    I_SLICE = 2,
} FH_ENC_SLICE_TYPE;

typedef struct
{
    FH_UINT32 chan;
    FH_ADDR start;
    FH_ENC_SLICE_TYPE frame_type;
    FH_UINT32 length;
    FH_UINT64 time_stamp;
    FH_UINT32 nalu_cnt;
    FH_ENC_STREAM_NALU nalu[PAE_MAX_NALU_CNT];
} FH_ENC_STREAM_ELEMENT;

typedef struct
{
    FH_UINT8 *base;
    FH_UINT32 size;
} FH_ENC_MEM;

typedef struct
{
    FH_BOOL water_enable;
    FH_ENC_MEM water_info;
} FH_WATER_INFO;

typedef struct
{
    FH_UINT32 enable;
    FH_ROTATE_OPS rotate;
} FH_ROTATE;

typedef enum { FH_ROI_L0 = 0, FH_ROI_L1 = 1, FH_ROI_L2 = 2, FH_ROI_L3 = 3 } FH_ROI_LEVEL;

typedef struct
{
    FH_UINT32 enable;
    FH_UINT32 qp;
    FH_AREA area;
    FH_ROI_LEVEL level;
} FH_ROI;

typedef enum {
    FH_REF_MODE_1X = 0,
    FH_REF_MODE_2X = 1,
    FH_REF_MODE_4X = 2,
} FH_REF_MODE_OPS;

typedef enum {
    FH_CAVLC = 0,
    FH_CABAC = 1,
} FH_ENTROPY_MODE;

typedef enum {
    FH_IDC_0 = 0,
    FH_IDC_1 = 1,
    FH_IDC_2 = 2,
} FH_CACBC_INIT_IDC;

typedef struct
{
    FH_ENTROPY_MODE entropy_coding_mode;
    FH_CACBC_INIT_IDC cabac_init_idc;
} FH_ENTROPY_OPS;

typedef struct
{
    FH_UINT32 deblocking_filter;   // 0,1
    FH_UINT32 disable_deblocking;  // 0,1,2
    FH_UINT32 slice_alpha;         //-6~+6 :  S4
    FH_UINT32 slice_beta;          //-6~+6 :   S4
} FH_DEBLOCKING_FILTER_PARAM;

typedef enum {
    FH_NONE   = 0,
    FH_LUMMA  = 1,
    FH_CHORMA = 2,
    FH_Y_C    = 3,
} FH_INTERMBSCE_OPS;

typedef struct
{
    FH_UINT32 enable;
    FH_UINT32 slicesplit;
} FH_SLICE_SPLIT;

typedef struct
{
    FH_MEM_INFO mem_info;
    FH_UINT32 user_w;
    FH_UINT32 user_h;
} FH_ENC_CHN_INFO;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

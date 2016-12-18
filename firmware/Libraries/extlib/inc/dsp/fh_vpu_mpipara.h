#ifndef __FH_VPU_MPIPARA_H__
#define __FH_VPU_MPIPARA_H__

#include "types/type_def.h"
#include "fh_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct
{
    FH_SIZE vi_size;
} FH_VPU_SIZE;

typedef enum {
    VPU_MODE_ISP = 0, /**< 从ISP输入模式*/
    VPU_MODE_MEM = 1, /**< 从MEM输入模式*/
} FH_VPU_VI_MODE;

/// VPU 输出模式
typedef enum {
    VPU_VOMODE_BLK  = 0, /**<yuv for pae*/
    VPU_VOMODE_SCAN = 1, /**<yuv for user*/
} FH_VPU_VO_MODE;

typedef struct
{
    FH_SIZE vpu_chn_size;
} FH_VPU_CHN_CONFIG;

typedef struct
{
    FH_SIZE pic_size;  /**< 图片尺寸 */
    FH_ADDR yluma;     /**< 图片数据的luma地址 */
    FH_ADDR chroma;    /**< 图片数据的chroma地址 */
    FH_UINT32 ystride; /**< 图片数据的luma stride */
    FH_UINT32 cstride; /**< 图片数据的 chroma stride */
} FH_VPU_USER_PIC;

/// YUV格式
typedef enum {
    PIXEL_FORMAT_MONOCHROME = 0, /**<单色*/
    PIXEL_FORMAT_420        = 1, /**<420*/
    PIXEL_FORMAT_422        = 2, /**<422*/
    PIXEL_FORMAT_444        = 3  /**<444*/
} FH_VPU_PIXELFORMAT;

typedef struct
{
    FH_VPU_PIXELFORMAT pixelFormat; /**< 像素格式 */
    FH_UINT32 stride;               /**< 图像跨度 */
    FH_SIZE size;                   /**< 图像数据长度 */
    FH_MEM_INFO yluma;              /**< luma地址*/
    FH_MEM_INFO chroma;             /**< chroma 地址*/
    FH_UINT64 time_stamp;           /**< 时间戳*/
} FH_VPU_STREAM;

typedef struct
{
    FH_UINT32 masaic_enable; /**<使能，1显示 0 不显示 */
    FH_UINT32 masaic_size;   /**<尺寸0: 16x16，1: 32x32 */
} FH_MASK_MASAIC;

typedef struct
{
    FH_BOOL mask_enable[MAX_MASK_AREA]; /**< 覆盖使能 */
    FH_AREA area_value[MAX_MASK_AREA];  /**< 屏蔽区域 */
    FH_UINT32 color;                    /**< 显示颜色 */
    FH_MASK_MASAIC masaic;              /**< 马赛克属性 */
} FH_VPU_MASK;

typedef struct
{
    FH_UINT32 alpha_value; /**< 图形层的图像格式是ARGB1555格式，当A=1的时候此处的Alpha起作用，取值[0..127] 取值越小，越透明 */
    FH_UINT32 dtvmode;     /*0为SDTV模式，1为HDTV模式*/
    FH_UINT32 rgbmode;     /*0为stdio RGB，1为computer RGB*/
    FH_SIZE logo_size;     /**< logo大小 */
} FH_LOGO_CFG;

typedef struct
{
    FH_UINT32 logo_enable;  /**< 图片叠加使能 */
    FH_UINT8 *logo_addr;    /**< logo数据的物理地址 */
    FH_LOGO_CFG logo_cfg;   /**< logo 配置 */
    FH_POINT logo_startpos; /**< logo 左上角起始点 */
    FH_POINT logo_cutpos;   /**<logo切割显示起始点 */
    FH_UINT32 stride_value; /**<行长度 */
} FH_VPU_LOGO;

typedef enum fh_OSDROTATE_E {
    OSD_ROTATE_NONE = 0,
    OSD_ROTATE_90   = 1,
    OSD_ROTATE_180  = 2,
    OSD_ROTATE_270  = 3,
    OSD_ROTATE_BUTT
} FH_OSD_ROTATE;
typedef struct
{
    FH_UINT32 Vamp;      /*水平放大一倍*/
    FH_UINT32 Hamp;      /*垂直放大一倍*/
    FH_UINT32 txtrotate; /*整个字符旋转*/
    FH_UINT32 txtw;      /*字符宽度*/
    FH_UINT32 txth;      /*字符高度,只有txt2模式时设置*/
    FH_UINT32 fontsize;  /*字体占的内存大小*/
    FH_UINT32 fontw;
    FH_UINT32 fonth;
} FH_OSD_CFG;

typedef struct
{
    FH_UINT32 norcolor; /*正常的字符颜色*/
    FH_UINT32 invcolor; /*反色的字符颜色*/
} FH_OSD_COLOR;

typedef struct
{
    FH_UINT32 invert_enable;
    FH_UINT32 text_idex;
    FH_UINT32 invert_word[MAX_INVERT_CNT];
} FH_INVERT_CTRL;

typedef struct
{
    FH_UINT32 idex; /*字符标记，0为txt0,1为text1,2为text2*/
    FH_UINT32 osd_enable;

    FH_UINT8 *osdtxtaddr;      /*字符的起始地址*/
    FH_UINT8 *osdfontaddr;     /*字符库的地址*/
    FH_INVERT_CTRL invert_ctl; /*字符颜色反色控制*/
    FH_OSD_CFG osd_cfg;        /**< 配置 */
    FH_OSD_COLOR osd_color;    /**< 字符颜色 */
    FH_OSD_COLOR osd_blgcolor; /**< 背景颜色 */
    FH_POINT osd_startpos;     /**< 图像中显示的位置 */
} FH_VPU_OSD;

typedef struct
{
    FH_UINT32 frame_id;
    FH_ADDR_INFO ymean;
    FH_ADDR_INFO cmean;
} FH_VPU_YCMEAN;

typedef struct
{
    FH_AREA vpu_crop_area;
} FH_VPU_CROP;

typedef struct
{
    FH_UINT16 CNREdgeT;       /*边界阈值强度U10*/
    FH_UINT16 CNREdgeT1;      /*边界阈值强度U10*/
    FH_UINT16 CNRYDwSkipMode; /*CNR中使用的YNR信息下采样模式U2*/
    FH_UINT16 CNRSigma;       /*噪声方差U8*/
    FH_UINT16 CNRAlpha;       /*噪声级别U4*/
    FH_UINT16 YNRTh;          /*YNR中的阈值U10*/
} FH_VPU_YCNR_PARAMS;

typedef struct
{
    FH_UINT8 APCPGain; /*总体APC正向增益U4.4*/
    FH_UINT8 APCNGain; /*总体APC负向增益U4.4*/
    FH_UINT8 MergeSel; /*细节增强和边界锐化合并模式U1*/
    FH_UINT8 DEPGain;  /*细节增强正向增益U7*/
    FH_UINT8 DENGain;  /*细节增强负向增益U4.4*/
    FH_UINT16 DESTHL;  /* 细节增强Slice LOW门限值U10*/
    FH_UINT16 DESTHH;  /* 细节增强Slice high门限值U10*/
    FH_UINT8 ESPGain;  /*边界锐化正向增益*/
    FH_UINT8 ESNGain;  /*边界锐化负向增益*/
    FH_UINT16 ESSTHL;  /*边界锐化 slice low门限值*/
    FH_UINT16 ESSTHH;  /*边界锐化slice HIGH门限值*/
} FH_VPU_APC_PARAMS;

typedef struct
{
    FH_UINT8 *base;
    FH_UINT32 size;
    FH_UINT32 user_w;
    FH_UINT32 user_h;
} FH_VPU_CHN_INFO;

typedef struct
{
    FH_UINT32 base;
    void *vbase;
    FH_UINT32 size;
} FH_PKG_INFO;

typedef enum {
    FH_VPU_MOD_MASK      = (1 << 1),  /**<隐私屏蔽 */
    FH_VPU_MOD_LOGO      = (1 << 3),  /**<图像层 */
    FH_VPU_MOD_STAT_MEAN = (1 << 4),  /**<统计输出 */
    FH_VPU_MOD_OSD0      = (1 << 5),  /**<文字层0 */
    FH_VPU_MOD_OSD1      = (1 << 6),  /**<文字层1 */
    FH_VPU_MOD_OSD2      = (1 << 7),  /**<文字层2 */
    FH_VPU_MOD_DITHER    = (1 << 15), /**<防抖动*/
    FH_VPU_MOD_CNR       = (1 << 16), /**<色度降噪 */
    FH_VPU_MOD_YNR       = (1 << 17), /**<亮度降噪 */
    FH_VPU_MOD_APC       = (1 << 18), /**<锐化 */
    FH_VPU_MOD_PURPLE    = (1 << 19), /**<去紫边*/
} FH_VPU_MODULE;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

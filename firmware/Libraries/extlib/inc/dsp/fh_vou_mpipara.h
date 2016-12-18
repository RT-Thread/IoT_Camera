#ifndef __FH_VOU_MPIPARA_H__
#define __FH_VOU_MPIPARA_H__

#include "types/type_def.h"
#include "fh_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

typedef struct
{
    FH_UINT32 mode; /**< 表示VOU 输出模式 PAL制720 = 0 ，NTSC制 = 1,960暂不支持 */
} FH_VOU_PIC_CONFIG;

typedef struct
{
    FH_SIZE vou_size;
} FH_VOU_PIC_SIZE;

typedef struct
{
    FH_ADDR yaddr;
    FH_ADDR caddr;
    FH_UINT32 ystride;
    FH_UINT32 cstride;
    FH_SIZE vou_size;
} FH_VOU_PIC_INFO;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif

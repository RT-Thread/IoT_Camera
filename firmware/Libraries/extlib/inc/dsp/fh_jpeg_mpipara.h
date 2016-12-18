#ifndef __FH_JPEG_MPIPARA_H__
#define __FH_JPEG_MPIPARA_H__

#include "types/type_def.h"
#include "fh_common.h"

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

typedef struct
{
    FH_SINT32 QP;    // 0-98
    FH_SINT32 rate;  // JPEG 完成速度控制 28bit
    FH_ROTATE_OPS rotate;
} FH_JPEG_CONFIG;

typedef struct
{
    FH_SINT32 frame_id;
    FH_ADDR YADDR;
    FH_ADDR CADDR;
    FH_SIZE jpeg_size;
} FH_JPEG_FRAME_INFO;

typedef struct
{
    FH_SINT32 frame_id;
    FH_SIZE size;
    FH_ADDR_INFO stream;
} FH_JPEG_STREAM_INFO;

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif


#ifndef __PSMUX_LIB_H__
#define __PSMUX_LIB_H__

#ifdef __cplusplus
extern "C" {
#endif

/* video stream type */
#define STREAM_TYPE_VIDEO_H264 0x1b /* 标准H264       */

/* 输入帧类型，对应frame_type */
#define FRAME_TYPE_UNDEF (-1)     /* 未定义的帧类型 */
#define FRAME_TYPE_VIDEO_IFRAME 3 /* 视频数据 I 帧 */
#define FRAME_TYPE_VIDEO_PFRAME 1 /* 视频数据 P 帧 */
#define FRAME_TYPE_VIDEO_BFRAME 0 /* 视频数据 B 帧 */

/* 状态码：小于零表示有错误(0x80000000开始)，零表示失败，大于零表示成功  */
#define PSMUX_LIB_S_OK 0x00000001          /* 成功           */
#define PSMUX_LIB_S_FAIL 0x00000000        /* 失败           */
#define PSMUX_LIB_E_PARA_NULL 0x80000000   /* 参数指针为空 */
#define PSMUX_LIB_E_MEM_OVER 0x80000001    /* 内存溢出     */
#define PSMUX_LIB_E_STREAM_TYPE 0x80000003 /* 流类型错误   */

#define MAX_PES_PACKET_LEN 65496

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef int HRESULT;
#endif /*!_HRESULT_DEFINED*/

typedef struct _PSMUX_ES_INFO_
{
    unsigned int max_byte_rate;     /* 码率，以byte为单位*/
    unsigned int max_packet_len;    /* 最大 pes 包长度 */
    unsigned int video_stream_type; /* 输入视频流类型 */
    unsigned int set_frame_end_flg; /* 是否在pes包头的填充字节里设置帧或nalu结束开始标记 */
} PSMUX_ES_INFO;

/* 复合器参数 */
typedef struct _PSMUX_PARAM_
{
    unsigned int buffer_size;
    unsigned char *buffer;
    PSMUX_ES_INFO info;
} PSMUX_PARAM;

/* 数据块处理参数 */
typedef struct _PSMUX_UNIT_
{
    unsigned int frame_type;    /* 输入帧类型                           */
    unsigned int is_first_unit; /* 是否是一帧的第一个unit。标准H.264每帧会分成多个unit*/
                                /* 其余编码每帧都只有一个unit */
    unsigned int is_last_unit;  /* 是否是一帧的最后一个unit       */
    unsigned int is_key_frame;  /* 是否关键数据(I帧)                    */
    unsigned int is_unit_start; /* 若是一个nalu或一帧的第一段数据，则置1，若送进的是完整的一帧或一个nalu也置1*/
    unsigned int is_unit_end; /* 若是一个nalu或一帧的最后一段数据，则置1，若送进的是完整的一帧或一个nalu也置1*/
    unsigned long long sys_clk_ref; /* 系统参考时钟，以 1/45000 秒为单位    */
    unsigned long long ptime_stamp; /* 该帧在接收端的显示时标，单位同上     */
    unsigned int frame_num;         /* 当前帧号       */
    unsigned char *unit_in_buf;     /* 输入 unit 指针 */
    unsigned int unit_in_len;       /* 输出 unit 长度 */

    unsigned char *out_buf;    /* 输出缓冲区           */
    unsigned int out_buf_len;  /* 输出缓冲区长度       */
    unsigned int out_buf_size; /* 输出缓冲区大小       */

    /* 码流信息 */
    unsigned int encrypt; /* 是否加密                             */

} PSMUX_PROCESS_PARAM;

/******************************************************************************
* 功  能：获取所需内存大小
* 参  数：param - 参数结构指针
* 返回值：返回错误码
* 备  注：参数结构中 buffer_size变量用来表示所需内存大小
******************************************************************************/
HRESULT PSMUX_GetMemSize(PSMUX_PARAM *param);

/******************************************************************************
* 功  能：创建PSMUX模块
* 参  数：param    - 参数结构指针
*         **handle  - 返回PSMUX模块句柄
* 返回值：返回错误码
******************************************************************************/
HRESULT PSMUX_Create(PSMUX_PARAM *param, void **handle);

/******************************************************************************
* 功  能：复合一段数据块
* 参  数：handle - 句柄(handle由PSMUX_Create返回)
* 返回值：返回错误码
******************************************************************************/
HRESULT PSMUX_Process(void *handle, PSMUX_PROCESS_PARAM *param);

/******************************************************************************
* 功  能：重置参考数据
* 参  数：handle - 句柄(handle由PSMUX_Create返回)
*         info   - 参考数据句柄
* 返回值：返回错误码
******************************************************************************/
HRESULT PSMUX_ResetStreamInfo(void *handle, PSMUX_ES_INFO *info);

#ifdef __cplusplus
}
#endif

#endif /* __PSMUX_LIB_H__ */

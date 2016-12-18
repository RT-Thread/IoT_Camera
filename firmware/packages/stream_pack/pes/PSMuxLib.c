
//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "PSMuxLib.h"
#if (1)
#define STUFF_BYTE_0XFF 0xff
#define PROGRAM_STREAM_PSH_TAG 0xba
#define PROGRAM_STREAM_PSM_TAG 0xbc
#define PES_VIDEO_BASE 0xe0

#define MIN_PES_HEAD_STUFF_LEN 2

typedef struct _PES_PACKET_INFO
{
    unsigned int stream_id;
    unsigned int add_pts;
    unsigned int total_data_len;
    unsigned int rest_data_len;
    unsigned int payload_len;
} PES_PACK_INFO;

typedef struct _PROGRAM_STREAM_INFO_
{
    unsigned int video_stream_id;
    unsigned int max_byte_rate; /* 码率，以byte为单位*/
    unsigned int max_pes_len;   /* 最大pes长度*/
    unsigned int set_frame_end_flg;
    unsigned int video_stream_type; /* 输入视频流类型 */
    PES_PACK_INFO pes_cur;
} PS_STREAM_INFO;

//符合mpeg2 part1定义的crc32码表
unsigned int PSMUX_CTX[257] = {
    0x00000000, 0xb71dc104, 0x6e3b8209, 0xd926430d, 0xdc760413, 0x6b6bc517, 0xb24d861a, 0x0550471e, 0xb8ed0826,
    0x0ff0c922, 0xd6d68a2f, 0x61cb4b2b, 0x649b0c35, 0xd386cd31, 0x0aa08e3c, 0xbdbd4f38, 0x70db114c, 0xc7c6d048,
    0x1ee09345, 0xa9fd5241, 0xacad155f, 0x1bb0d45b, 0xc2969756, 0x758b5652, 0xc836196a, 0x7f2bd86e, 0xa60d9b63,
    0x11105a67, 0x14401d79, 0xa35ddc7d, 0x7a7b9f70, 0xcd665e74, 0xe0b62398, 0x57abe29c, 0x8e8da191, 0x39906095,
    0x3cc0278b, 0x8bdde68f, 0x52fba582, 0xe5e66486, 0x585b2bbe, 0xef46eaba, 0x3660a9b7, 0x817d68b3, 0x842d2fad,
    0x3330eea9, 0xea16ada4, 0x5d0b6ca0, 0x906d32d4, 0x2770f3d0, 0xfe56b0dd, 0x494b71d9, 0x4c1b36c7, 0xfb06f7c3,
    0x2220b4ce, 0x953d75ca, 0x28803af2, 0x9f9dfbf6, 0x46bbb8fb, 0xf1a679ff, 0xf4f63ee1, 0x43ebffe5, 0x9acdbce8,
    0x2dd07dec, 0x77708634, 0xc06d4730, 0x194b043d, 0xae56c539, 0xab068227, 0x1c1b4323, 0xc53d002e, 0x7220c12a,
    0xcf9d8e12, 0x78804f16, 0xa1a60c1b, 0x16bbcd1f, 0x13eb8a01, 0xa4f64b05, 0x7dd00808, 0xcacdc90c, 0x07ab9778,
    0xb0b6567c, 0x69901571, 0xde8dd475, 0xdbdd936b, 0x6cc0526f, 0xb5e61162, 0x02fbd066, 0xbf469f5e, 0x085b5e5a,
    0xd17d1d57, 0x6660dc53, 0x63309b4d, 0xd42d5a49, 0x0d0b1944, 0xba16d840, 0x97c6a5ac, 0x20db64a8, 0xf9fd27a5,
    0x4ee0e6a1, 0x4bb0a1bf, 0xfcad60bb, 0x258b23b6, 0x9296e2b2, 0x2f2bad8a, 0x98366c8e, 0x41102f83, 0xf60dee87,
    0xf35da999, 0x4440689d, 0x9d662b90, 0x2a7bea94, 0xe71db4e0, 0x500075e4, 0x892636e9, 0x3e3bf7ed, 0x3b6bb0f3,
    0x8c7671f7, 0x555032fa, 0xe24df3fe, 0x5ff0bcc6, 0xe8ed7dc2, 0x31cb3ecf, 0x86d6ffcb, 0x8386b8d5, 0x349b79d1,
    0xedbd3adc, 0x5aa0fbd8, 0xeee00c69, 0x59fdcd6d, 0x80db8e60, 0x37c64f64, 0x3296087a, 0x858bc97e, 0x5cad8a73,
    0xebb04b77, 0x560d044f, 0xe110c54b, 0x38368646, 0x8f2b4742, 0x8a7b005c, 0x3d66c158, 0xe4408255, 0x535d4351,
    0x9e3b1d25, 0x2926dc21, 0xf0009f2c, 0x471d5e28, 0x424d1936, 0xf550d832, 0x2c769b3f, 0x9b6b5a3b, 0x26d61503,
    0x91cbd407, 0x48ed970a, 0xfff0560e, 0xfaa01110, 0x4dbdd014, 0x949b9319, 0x2386521d, 0x0e562ff1, 0xb94beef5,
    0x606dadf8, 0xd7706cfc, 0xd2202be2, 0x653deae6, 0xbc1ba9eb, 0x0b0668ef, 0xb6bb27d7, 0x01a6e6d3, 0xd880a5de,
    0x6f9d64da, 0x6acd23c4, 0xddd0e2c0, 0x04f6a1cd, 0xb3eb60c9, 0x7e8d3ebd, 0xc990ffb9, 0x10b6bcb4, 0xa7ab7db0,
    0xa2fb3aae, 0x15e6fbaa, 0xccc0b8a7, 0x7bdd79a3, 0xc660369b, 0x717df79f, 0xa85bb492, 0x1f467596, 0x1a163288,
    0xad0bf38c, 0x742db081, 0xc3307185, 0x99908a5d, 0x2e8d4b59, 0xf7ab0854, 0x40b6c950, 0x45e68e4e, 0xf2fb4f4a,
    0x2bdd0c47, 0x9cc0cd43, 0x217d827b, 0x9660437f, 0x4f460072, 0xf85bc176, 0xfd0b8668, 0x4a16476c, 0x93300461,
    0x242dc565, 0xe94b9b11, 0x5e565a15, 0x87701918, 0x306dd81c, 0x353d9f02, 0x82205e06, 0x5b061d0b, 0xec1bdc0f,
    0x51a69337, 0xe6bb5233, 0x3f9d113e, 0x8880d03a, 0x8dd09724, 0x3acd5620, 0xe3eb152d, 0x54f6d429, 0x7926a9c5,
    0xce3b68c1, 0x171d2bcc, 0xa000eac8, 0xa550add6, 0x124d6cd2, 0xcb6b2fdf, 0x7c76eedb, 0xc1cba1e3, 0x76d660e7,
    0xaff023ea, 0x18ede2ee, 0x1dbda5f0, 0xaaa064f4, 0x738627f9, 0xc49be6fd, 0x09fdb889, 0xbee0798d, 0x67c63a80,
    0xd0dbfb84, 0xd58bbc9a, 0x62967d9e, 0xbbb03e93, 0x0cadff97, 0xb110b0af, 0x060d71ab, 0xdf2b32a6, 0x6836f3a2,
    0x6d66b4bc, 0xda7b75b8, 0x035d36b5, 0xb440f7b1, 0x00000001};

/******************************************************************************
* 功  能：生成crc32校验码，参考ffmpeg
* 参  数：  buffer  - 生成crc码的缓冲区
*           length  - 生成crc码的缓冲区长度
* 返回值：crc校验码
******************************************************************************/
static unsigned int PSMUX_mpeg2_crc(const unsigned char *buffer, unsigned int length)
{
    const unsigned char *end = buffer + length;
    unsigned int crc         = 0xffffffff;

    while (buffer < end)
    {
        crc = PSMUX_CTX[((unsigned char)crc) ^ *buffer++] ^ (crc >> 8);
    }
    return crc;
}

/******************************************************************************
* 功  能：根据 PES_PACK_INFO *pes 内的信息在 buffer 里生成 PES 头数据
* 参  数：  buffer      - PES 头缓冲区
*           pes         - 当前处理的pes
*           prc         - 当前处理的单元
*           pes         - 输入输出参数结构体
* 返回值：PES 头数据的长度
******************************************************************************/
unsigned int PSMUX_create_pes_header(unsigned char *buffer, PES_PACK_INFO *pes, PSMUX_PROCESS_PARAM *prc,
                                     PS_STREAM_INFO *prg)
{
    int i, pes_header_stuff_len = 0;
    unsigned int tmp;
    unsigned int pes_header_basic_len = 9;
    unsigned int pes_header_ext_len   = 0;
    unsigned int pos, stuff_flag = 0xff;
    unsigned int pes_header_len_syt;
    unsigned int pes_heder_fix_len;
    unsigned int pes_priority = (prc->frame_type != FRAME_TYPE_VIDEO_BFRAME);
    unsigned long long pts;

    if ((pes->rest_data_len == pes->total_data_len) && prc->is_unit_start)
    {
        stuff_flag &= 0xfd;
    }

    // 计算 pes 头字节和填充字节的长度
    if (pes->add_pts)
    {
        pes_header_ext_len += 5;
    }
    pes_heder_fix_len = pes_header_basic_len + pes_header_ext_len + MIN_PES_HEAD_STUFF_LEN;
    tmp               = pes_heder_fix_len + pes->rest_data_len;

    if (tmp > prg->max_pes_len)
    {
        pes_header_stuff_len = 2;
        pes_header_ext_len += pes_header_stuff_len;
        pes->payload_len = prg->max_pes_len - pes_header_basic_len - pes_header_ext_len;
    }
    else
    {
        // pes_header_stuff_len 至少为2，且要保证 pes 包 4 byte 对齐
        if ((tmp == prg->max_pes_len) || ((tmp & 3) == 0))
        {
            pes_header_stuff_len = 2;
        }
        else
        {
            pes_header_stuff_len = 2 + (4 - (tmp & 3));
        }
        pes_header_ext_len += pes_header_stuff_len;
        pes->payload_len = pes->rest_data_len;

        if (prc->is_unit_end)
        {
            stuff_flag &= 0xfe;
        }

        if (prc->is_last_unit && prc->is_unit_end)
        {
            stuff_flag &= 0xfb;
        }
    }

    pes_header_len_syt = pes_header_basic_len + pes_header_ext_len + pes->payload_len - 6;

    // 生成 pes 头数据
    buffer[0] = 0x00;
    buffer[1] = 0x00;
    buffer[2] = 0x01;
    buffer[3] = (unsigned char)(pes->stream_id);

    buffer[4] = (unsigned char)((pes_header_len_syt >> 8) & 0xff);
    buffer[5] = (unsigned char)(pes_header_len_syt & 0xff);

    buffer[6] = (1 << 7)                         //  2 - check bits '10'
                | ((prc->encrypt & 0x03) << 4)   //  2 - PES_scrambling_control(0)
                | ((pes_priority & 1) << 3)      //  1 - PES_priority(0)
                | (((~stuff_flag) << 1) & 0x04)  //  1 - data_alignment_indicator(0)
                | 0                              //  1 - copyright(0)
                | 0;                             //  1 - original_or_copy(0)

    buffer[7] = (((unsigned char)pes->add_pts) << 7);
    //  2 - PTS_DTS_flags()
    //  1 - ESCR_flag(0)
    //  1 - ES_rate_flag(0)
    //  1 - DSM_trick_mode_flag(0)
    //  1 - additional_copy_info_flag(0)
    //  1 - PES_CRC_flag(0)
    //  1 - PES_extension_flag()

    buffer[8] = (unsigned char)pes_header_ext_len;  //  8 - PES_header_data_length

    pos = 9;
    if (pes->add_pts)
    {
        pts           = prc->ptime_stamp;
        buffer[pos++] = (pts >> 29 & 0x0e) | 0x21;  //  4 - '0010'
                                                    //  3 - PTS [32..30]
                                                    //  1 - marker_bit
        buffer[pos++] = (pts >> 22);                //  8 - PTS [29..22]
        buffer[pos++] = (pts >> 14 & 0xfe) | 0x01;  //  7 - PTS [21..15]
                                                    //  1 - marker_bit
        buffer[pos++] = (pts >> 7);                 //  8 - PTS [14..7]
        buffer[pos++] = (pts << 1 & 0xfe) | 0x01;   //  7 - PTS [6..0]
                                                    //  1 - marker_bit
    }

    if (prg->set_frame_end_flg)
    {
        for (i = 0; i < pes_header_stuff_len - 1; i++)
        {
            buffer[pos++] = STUFF_BYTE_0XFF;
        }
        buffer[pos++] = (unsigned char)stuff_flag;
    }
    else
    {
        for (i = 0; i < pes_header_stuff_len; i++)
        {
            buffer[pos++] = STUFF_BYTE_0XFF;
        }
    }
    return pos;
}

/******************************************************************************
* 功  能：根据 byte_rate 和 scr 信息在 buffer 里生成 PS 头数据
* 参  数：  buffer      - PS 头缓冲区
*           byte_rate   - PS流码率
*           scr         - 系统参考时钟
*			frame_num	- 帧号
* 返回值：PS 头数据的长度
******************************************************************************/
int PSMUX_fill_PSH(unsigned char *buffer, PSMUX_PROCESS_PARAM *prc, PS_STREAM_INFO *prg)
{
    unsigned int mux_rate   = 0;
    unsigned int stream_num = 4;
    int pos                 = 0;

    mux_rate = prg->max_byte_rate / 50 + 1;

    buffer[pos++] = 0x00;
    buffer[pos++] = 0x00;
    buffer[pos++] = 0x01;
    buffer[pos++] = PROGRAM_STREAM_PSH_TAG;  // 32bits	0x 00 00 01 BA

    buffer[pos++] = 0x40                                  // 2bits	MPEG2标志 "01"
                    | ((prc->sys_clk_ref >> 27) & 0x38)   // 3bits	system_clock_reference_base [32..30]
                    | 0x04                                // 1bits	marker_bit
                    | ((prc->sys_clk_ref >> 28) & 0x03);  // 2bits	system_clock_reference_base [29..28]
    buffer[pos++] = (prc->sys_clk_ref >> 20) & 0xff;      // 8bits	system_clock_reference_base [27..20]
    buffer[pos++] = ((prc->sys_clk_ref >> 12) & 0xf8)     // 5bits	system_clock_reference_base [19..15]
                    | 0x04                                // 1bits	marker_bit
                    | ((prc->sys_clk_ref >> 13) & 0x03);  // 2bits	system_clock_reference_base [14..13]
    buffer[pos++] = (prc->sys_clk_ref >> 5) & 0xff;       // 8bits	system_clock_reference_base [12..5]
    buffer[pos++] = ((prc->sys_clk_ref << 3) & 0xf0)      // 5bits	system_clock_reference_base [4..0]
                    | 0x04                                // 1bits	marker_bit
                    | 0x00;                               // 2bits	system_clock_reference_ext [8..7]
    buffer[pos++] = 0x01;                                 // 7bits	system_clock_reference_ext [6..0]
    // 1bits	marker_bit
    buffer[pos++] = (mux_rate >> 14) & 0xff;  // 8bits	mux_rate [21..14]
    buffer[pos++] = (mux_rate >> 6) & 0xff;   // 8bits	mux_rate [13..6]
    buffer[pos++] = (mux_rate << 2)           // 6bits	mux_rate [5..0]
                    | 0x03;                   // 2bits	marker_bit

    buffer[pos++] = 0xFE;             // 5bits	reserved, 3bits stuffing_len = 6;
    buffer[pos++] = STUFF_BYTE_0XFF;  // stuffing_byte
    buffer[pos++] = STUFF_BYTE_0XFF;  // stuffing_byte

    if (prg->set_frame_end_flg)
    {
        buffer[pos++] = (prc->frame_num >> 24) & 0xff;
        buffer[pos++] = (prc->frame_num >> 16) & 0xff;
        buffer[pos++] = (prc->frame_num >> 8) & 0xff;
        buffer[pos++] = prc->frame_num & 0xff;
    }
    else
    {
        buffer[pos++] = STUFF_BYTE_0XFF;
        buffer[pos++] = STUFF_BYTE_0XFF;
        buffer[pos++] = STUFF_BYTE_0XFF;
        buffer[pos++] = STUFF_BYTE_0XFF;
    }

    if (prc->is_key_frame)
    {
        // system header
        buffer[pos++] = 0x00;
        buffer[pos++] = 0x00;
        buffer[pos++] = 0x01;
        buffer[pos++] = 0xbb;  // 32bits 0x000001BB

        buffer[pos++] = (6 + 3 * stream_num) >> 8;    // 8bits	header_length_high_8_bits
        buffer[pos++] = (6 + 3 * stream_num) & 0xff;  // 8bits	header_length_low_8_bits

        buffer[pos++] = 0x80                          // 1bits marker_bit
                        | ((mux_rate >> 15) & 0x7f);  // 7bits	rate_bound [21..15]
        buffer[pos++] = (mux_rate >> 7) & 0xff;       // 8bits	rate_bound [14..7]
        buffer[pos++] = ((mux_rate << 1) & 0xfe)      // 7bits	rate_bound [6..0]
                        | 0x01;                       // 1bits marker_bit

        buffer[pos++] = 0x04     // 6bits	audio_bound
                        | 0x00   // 1bits	fixed_flag
                        | 0x00;  // 1bits	csps_flag
        buffer[pos++] = 0x80     // 1bits	sys_audio_lock_flag
                        | 0x40   // 1bits	sys_video_lock_flag
                        | 0x20   // 1bits	marker_bit
                        | 0x01;  // 5bits	video_bound
        buffer[pos++] = 0x00     // 1bits packet_rate_restriction_flag
                        | 0x7f;  // 7bits	reserved

        buffer[pos++] = 0xe0;      // stream_id
        buffer[pos++] = (0xc0      // 2 bits "11"
                         | 0x20);  // 1 bit P-STD_buffer_bound_scale, video must '1'
        buffer[pos++] =
            0x80;  // 13 bits P-STD_buffer_size_bound, if bound_scale is '0', unit is 128 bytes, or unit is 1024 bytes

        buffer[pos++] = 0xc0;      // stream_id
        buffer[pos++] = (0xc0      // 2 bits "11"
                         | 0x00);  // 1 bit P-STD_buffer_bound_scale, audio must '0'
        buffer[pos++] =
            0x08;  // 13 bits P-STD_buffer_size_bound, if bound_scale is '0', unit is 128 bytes, or unit is 1024 bytes

        buffer[pos++] = 0xbd;      // stream_id
        buffer[pos++] = (0xc0      // 2 bits "11"
                         | 0x20);  // 1 bit P-STD_buffer_bound_scale, video must '1'
        buffer[pos++] =
            0x80;  // 13 bits P-STD_buffer_size_bound, if bound_scale is '0', unit is 128 bytes, or unit is 1024 bytes

        buffer[pos++] = 0xbf;      // stream_id
        buffer[pos++] = (0xc0      // 2 bits "11"
                         | 0x20);  // 1 bit P-STD_buffer_bound_scale, video must '1'
        buffer[pos++] =
            0x80;  // 13 bits P-STD_buffer_size_bound, if bound_scale is '0', unit is 128 bytes, or unit is 1024 bytes
    }

    return pos;
}

/******************************************************************************
* 功  能：根据 PROGRAM_INFO *prg 信息在 buffer 里生成 PSM 数据
* 参  数：  buffer      - PSM 缓冲区
*           prg         - 节目流参考信息
*           prc         - 处理参数，主要用到全局时间
* 返回值：PSM 数据的长度
******************************************************************************/
int PSMUX_fill_PSM(unsigned char *buffer, PS_STREAM_INFO *prg, PSMUX_PROCESS_PARAM *prc)
{
    int pos, tmp_pos, tmp_len = 0;
    int crc;
    int len_pos;

    buffer[0] = 0x00;
    buffer[1] = 0x00;
    buffer[2] = 0x01;
    buffer[3] = PROGRAM_STREAM_PSM_TAG;

    //暂时填充pes包头长度，后面修改1
    buffer[4] = STUFF_BYTE_0XFF;
    buffer[5] = STUFF_BYTE_0XFF;

    buffer[6] = 0xe0;
    buffer[7] = STUFF_BYTE_0XFF;

    buffer[8] = (unsigned char)(tmp_len >> 8);
    buffer[9] = (unsigned char)(tmp_len);
    pos       = 10;

    //暂时填充pes包头长度，后面修改2
    tmp_pos       = pos;
    buffer[pos++] = STUFF_BYTE_0XFF;
    buffer[pos++] = STUFF_BYTE_0XFF;

    buffer[pos++] = prg->video_stream_type;
    buffer[pos++] = prg->video_stream_id;
    len_pos       = pos;
    tmp_len       = 0;
    pos += 2;
    buffer[len_pos++] = tmp_len >> 8;
    buffer[len_pos]   = tmp_len;

    //暂时填充修改2
    buffer[tmp_pos]     = (unsigned char)((pos - tmp_pos - 2) >> 8);
    buffer[tmp_pos + 1] = (unsigned char)(pos - tmp_pos - 2);

    //暂时填充修改1
    buffer[4] = (pos - 2) >> 8;
    buffer[5] = pos - 2;

    crc = PSMUX_mpeg2_crc(buffer, pos);

    buffer[pos++] = (unsigned char)(crc >> 24);
    buffer[pos++] = (unsigned char)(crc >> 16);
    buffer[pos++] = (unsigned char)(crc >> 8);
    buffer[pos++] = (unsigned char)(crc);

    return (pos);
}

/******************************************************************************
* 功  能：获取所需内存大小
* 参  数：param - 参数结构指针
* 返回值：返回错误码
* 备  注：参数结构中 buffer_size变量用来表示所需内存大小
******************************************************************************/
HRESULT PSMUX_GetMemSize(PSMUX_PARAM *param)
{
    if (param == NULL)
    {
        return PSMUX_LIB_E_PARA_NULL;
    }
    param->buffer_size = sizeof(PS_STREAM_INFO);
    return PSMUX_LIB_S_OK;
}

/******************************************************************************
* 功  能：创建PSMUX模块
* 参  数：param  	- 参数结构指针
*         **handle	- 返回PSMUX模块句柄
* 返回值：返回错误码
******************************************************************************/
HRESULT PSMUX_Create(PSMUX_PARAM *param, void **handle)
{
    PS_STREAM_INFO *prg = (PS_STREAM_INFO *)param->buffer;
    if (param == NULL)
    {
        return PSMUX_LIB_E_PARA_NULL;
    }
    if (param->buffer == NULL)
    {
        return PSMUX_LIB_E_PARA_NULL;
    }

    PSMUX_ResetStreamInfo((void *)prg, &param->info);

    prg->video_stream_id = PES_VIDEO_BASE;

    *handle = (void *)prg;
    return PSMUX_LIB_S_OK;
}

/******************************************************************************
* 功  能：重置参考数据
* 参  数：handle - 句柄(handle由PSMUX_Create返回)
*         info   - 参考数据句柄
* 返回值：返回错误码
******************************************************************************/
HRESULT PSMUX_ResetStreamInfo(void *handle, PSMUX_ES_INFO *info)
{
    PS_STREAM_INFO *prg = (PS_STREAM_INFO *)handle;

    if (handle == NULL || info == NULL)
    {
        return PSMUX_LIB_E_PARA_NULL;
    }

    prg->max_byte_rate     = info->max_byte_rate;     /* 码率，以byte为单位*/
    prg->max_pes_len       = info->max_packet_len;    /* 最大pes长度*/
    prg->video_stream_type = info->video_stream_type; /* 输入视频流类型 */
    prg->set_frame_end_flg = info->set_frame_end_flg;

    //最大pes包长度不能超过65496，且要为4的整数倍
    if (info->max_packet_len > MAX_PES_PACKET_LEN)
    {
        info->max_packet_len = MAX_PES_PACKET_LEN;
    }
    prg->max_pes_len = info->max_packet_len - (info->max_packet_len & 3);

    return PSMUX_LIB_S_OK;
}

/******************************************************************************
* 功  能：复合一段数据块
* 参  数：handle - 句柄(handle由PSMUX_Create返回)
*         param  - 处理单元参数
* 返回值：返回错误码
******************************************************************************/
HRESULT PSMUX_Process(void *handle, PSMUX_PROCESS_PARAM *param)
{
    PS_STREAM_INFO *prg = (PS_STREAM_INFO *)handle;
    PES_PACK_INFO *pes;
    int pos                 = 0;
    unsigned int packed_len = 0;
    unsigned char *buffer   = param->out_buf;

    if (param == NULL || prg == NULL)
    {
        return PSMUX_LIB_E_PARA_NULL;
    }
    pes = &prg->pes_cur;

    //根据输入处理 stream_id
    switch (param->frame_type)
    {
    case FRAME_TYPE_VIDEO_IFRAME:
    case FRAME_TYPE_VIDEO_PFRAME:
        pes->stream_id = prg->video_stream_id;
        break;
    default:
        return PSMUX_LIB_E_STREAM_TYPE;
    }

    if (param->is_first_unit && param->is_unit_start)
    {
        //当输入为视频P帧或者关键帧时，添加PSH
        if (param->is_key_frame ||
            (pes->stream_id == prg->video_stream_id && param->frame_type != FRAME_TYPE_VIDEO_BFRAME))
        {
            pos += PSMUX_fill_PSH(buffer, param, prg);
            if (pos >= param->out_buf_size)
            {
                return PSMUX_LIB_E_MEM_OVER;
            }
        }
        if (param->frame_type != FRAME_TYPE_VIDEO_IFRAME && param->frame_type != FRAME_TYPE_VIDEO_PFRAME &&
            param->is_first_unit && param->is_unit_start)
        {
            pos += PSMUX_fill_PSH(buffer, param, prg);
            if (pos >= param->out_buf_size)
            {
                return PSMUX_LIB_E_MEM_OVER;
            }
        }

        //当输入为关键帧时，添加PSM
        if (param->is_key_frame)
        {
            pos += PSMUX_fill_PSM(&buffer[pos], prg, param);
            if (pos >= param->out_buf_size)
            {
                return PSMUX_LIB_E_MEM_OVER;
            }
        }
    }

    pes->add_pts        = (param->is_first_unit && param->is_unit_start);
    pes->rest_data_len  = param->unit_in_len;
    pes->total_data_len = param->unit_in_len;
    pes->payload_len    = 0;

    //将输入数据打包
    do
    {
        pos += PSMUX_create_pes_header(&buffer[pos], pes, param, prg);

        if (pos + pes->payload_len > param->out_buf_size)
        {
            return PSMUX_LIB_E_MEM_OVER;
        }
        memcpy(&buffer[pos], &param->unit_in_buf[packed_len], pes->payload_len);
        packed_len += pes->payload_len;
        pos += pes->payload_len;
        pes->add_pts       = 0;  //除了第一个pes包外，其他包不添加pts和userdata
        pes->rest_data_len = pes->total_data_len - packed_len;

    } while (packed_len < pes->total_data_len);

    param->out_buf_len = pos;
    return PSMUX_LIB_S_OK;
}
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <rtthread.h>
#include <lwip/sockets.h>
#include "pes_pack.h"
#include "PSMuxLib.h"

#define MAX_ENC_CHAN 2
#define MAX_STREAM_ELEMENT_LEN 0x80000
#define DSP_SHARE_BUF_LEN 0x180000
#define PACK_OUT_BUF_LEN 0x80000
#define MAX_NALU_NUM 16
#define MAX_BYTE_RATE (256 * 1024)
#define BPS_VBR 0
#define BPS_CBR 1
#define I_FRAME_MODE 0x00001001
#define P_FRAME_MODE 0x00001003

struct stream_pack_info
{
    unsigned int channel_id;
    void *ps_handle;
    PSMUX_PARAM ps_mux_param;
    unsigned int nalu_num;
    struct vlcview_h264_nalu nalu[MAX_NALU_NUM];
    unsigned int frame_len;
    unsigned int picture_mode;
    unsigned long long time_stamp;
    unsigned int frame_num;
};

struct rec_pool_info
{
    unsigned char *addr;
    unsigned int write_index;
    unsigned int read_index;
    unsigned int total_len;
};

struct vlc_info
{
    int channel;
    int server_sock;
    int port;
    rt_thread_t thread_id;
    struct rt_mutex rec_pool_lock;
    char ip[80];
};

struct vlcview_info
{
    unsigned int channel_count;
    struct rec_pool_info rec_pool[MAX_ENC_CHAN];
    struct vlc_info vlc_info[MAX_ENC_CHAN];
};

static int g_cancel = 0;
static unsigned char g_rec_pool_buf[DSP_SHARE_BUF_LEN];
static struct stream_pack_info g_stream_pack_info[2];
static unsigned char g_pack_out_buf[PACK_OUT_BUF_LEN];
struct vlcview_info g_vlcview_info;

int init_stream_pack()
{
    unsigned int chan;

    memset((void *)g_stream_pack_info, 0, sizeof(g_stream_pack_info));
    for (chan = 0; chan < g_vlcview_info.channel_count; chan++)
    {
        g_stream_pack_info[chan].channel_id = chan;
    }

    return 0;
}

int deinit_stream_pack()
{
    return 0;
}

static void set_psmux_info(PSMUX_ES_INFO *pes_info)
{
    pes_info->video_stream_type = STREAM_TYPE_VIDEO_H264;
    pes_info->max_packet_len    = MAX_PES_PACKET_LEN;
    pes_info->set_frame_end_flg = 1;
    pes_info->max_byte_rate     = MAX_BYTE_RATE;
}

static int create_psmux_handle(struct stream_pack_info *pack_info)
{
    PSMUX_PARAM *psmux_param = &pack_info->ps_mux_param;

    pack_info->ps_handle = NULL;
    if (psmux_param->buffer && psmux_param->buffer_size)
    {
        rt_free(psmux_param->buffer);
        psmux_param->buffer      = NULL;
        psmux_param->buffer_size = 0;
    }

    set_psmux_info(&psmux_param->info);

    PSMUX_GetMemSize(psmux_param);
    if ((psmux_param->buffer = rt_malloc(psmux_param->buffer_size)) == NULL)
        return -1;

    memset(psmux_param->buffer, 0, psmux_param->buffer_size);

    if (PSMUX_Create(psmux_param, &pack_info->ps_handle) != PSMUX_LIB_S_OK)
    {
        pack_info->ps_handle = NULL;
        return -1;
    }

    return 0;
}

static int send_stream_to_rec_pool(unsigned int chan, unsigned char *stream_data, unsigned int len)
{
    unsigned int w, r;
    int err = 0;
    unsigned int spare_len, part1, part2;
    unsigned int total_len;
    char *p;
    struct rec_pool_info *rec_pool;

    if (chan < g_vlcview_info.channel_count)
        rec_pool = &(g_vlcview_info.rec_pool[chan]);
    else
        return -1;

    if (NULL == stream_data || 0 == len || 0 == rec_pool->total_len)
        return -2;

    if (rec_pool->addr == NULL)
        return -3;

    rt_mutex_take(&g_vlcview_info.vlc_info[chan].rec_pool_lock, RT_WAITING_FOREVER);

    if (rec_pool->total_len)
    {
        w         = rec_pool->write_index;
        r         = rec_pool->write_index;  //不判断读指针，循环覆盖
        total_len = rec_pool->total_len;
        p         = (char *)rec_pool->addr;
        spare_len = (r + total_len - w - 1) % total_len;

        if (len > spare_len)
        {
            printf("libvlcview: lost one frame\n");
            goto exit;
        }
        part1 = total_len - w;
        if (len > part1)
        {
            memcpy((unsigned char *)(p + w), (unsigned char *)stream_data, part1);
            part2 = len - part1;
            memcpy((unsigned char *)p, (unsigned char *)stream_data + part1, part2);
        }
        else
        {
            memcpy((unsigned char *)(p + w), (unsigned char *)stream_data, len);
        }
        rec_pool->write_index = (rec_pool->write_index + len) % total_len;
    }
exit:
    rt_mutex_release(&g_vlcview_info.vlc_info[chan].rec_pool_lock);
    return err;
}

static unsigned int video_ps_pack(PSMUX_PROCESS_PARAM psmux_param, struct stream_pack_info *pack_info)
{
    int j = 0, ret = 0;
    unsigned int chan;
    unsigned int ps_len    = 0;
    unsigned char *out_buf = g_pack_out_buf;

    if (NULL == pack_info)
        return 0;

    chan = pack_info->channel_id;

    psmux_param.encrypt = 0;
    for (j = 0; j < pack_info->nalu_num; j++)
    {
        psmux_param.unit_in_buf   = pack_info->nalu[j].start;
        psmux_param.unit_in_len   = pack_info->nalu[j].len;
        psmux_param.out_buf       = out_buf;
        psmux_param.out_buf_len   = 0;
        psmux_param.out_buf_size  = MAX_STREAM_ELEMENT_LEN;
        psmux_param.is_unit_start = 1;
        psmux_param.is_unit_end   = 1;
        psmux_param.is_first_unit = (j == 0);
        psmux_param.is_last_unit  = (j == (pack_info->nalu_num - 1));

        ret = PSMUX_Process(pack_info->ps_handle, &psmux_param);
        if (ret == PSMUX_LIB_S_OK)
        {
            ps_len += psmux_param.out_buf_len;
            out_buf += psmux_param.out_buf_len;
        }
        else
        {
            printf("libvlcview: create pes packet for nalu[%d] failed\n", j);
        }
    }

    if (ps_len & 3)
    {
        ps_len += (4 - (ps_len & 3));
    }
    send_stream_to_rec_pool(chan, g_pack_out_buf, ps_len);

    return ps_len;
}

static int do_stream_pack(struct stream_pack_info *pack_info)
{
    unsigned int chan       = pack_info->channel_id;
    unsigned int output_len = 0;
    unsigned long long clock;

    if (pack_info->nalu_num > 5 || pack_info->frame_len > MAX_STREAM_ELEMENT_LEN)
        return 0;

    clock = pack_info->time_stamp;

    PSMUX_PROCESS_PARAM psmux_param;
    if (pack_info->ps_handle != NULL && (g_vlcview_info.rec_pool[chan].total_len > 0))
    {
        memset(&psmux_param, 0, sizeof(psmux_param));
        if (pack_info->picture_mode == I_FRAME_MODE)
        {
            psmux_param.is_key_frame = 1;
            psmux_param.frame_type   = FRAME_TYPE_VIDEO_IFRAME;
            psmux_param.sys_clk_ref  = clock;
            psmux_param.ptime_stamp  = clock;
        }
        else if (pack_info->picture_mode == P_FRAME_MODE)
        {
            psmux_param.is_key_frame = 0;
            psmux_param.frame_type   = FRAME_TYPE_VIDEO_PFRAME;
            psmux_param.sys_clk_ref  = clock;
            psmux_param.ptime_stamp  = clock;
        }
        else
        {
            return -1;
        }

        psmux_param.frame_num = pack_info->frame_num;
        output_len            = video_ps_pack(psmux_param, pack_info);
        if (output_len == 0)
            return -1;
    }
    else
    {
        printf("libvlcview: Lost Record Frame\n");
    }
    return 0;
}

int vlcview_pes_stream_pack(int channel, struct vlcview_enc_stream_element stream_element)
{
    RT_ASSERT(channel <= 3);
    unsigned int i = 0;
    unsigned long long clock_90K;
    int err;

    clock_90K = stream_element.time_stamp * 9 / 100;
    if (stream_element.frame_type == VLCVIEW_ENC_H264_I_FRAME)
    {
        g_stream_pack_info[channel].picture_mode = I_FRAME_MODE;
    }
    else
    {
        g_stream_pack_info[channel].picture_mode = P_FRAME_MODE;
    }

    g_stream_pack_info[channel].time_stamp = clock_90K;
    g_stream_pack_info[channel].frame_num++;
    g_stream_pack_info[channel].frame_len = stream_element.frame_len;
    g_stream_pack_info[channel].nalu_num  = stream_element.nalu_count;

    for (i = 0; i < g_stream_pack_info[channel].nalu_num; i++)
    {
        g_stream_pack_info[channel].nalu[i].start = stream_element.nalu[i].start;
        g_stream_pack_info[channel].nalu[i].len   = stream_element.nalu[i].len;
    }

    if (g_stream_pack_info[channel].ps_handle == NULL)
    {
        err = create_psmux_handle(&g_stream_pack_info[channel]);
        if (err)
        {
            printf("create_psmux_handle ERROR!!!, chan=%d", g_stream_pack_info[channel].channel_id);
            return err;
        }
    }

    return do_stream_pack(&g_stream_pack_info[channel]);
}

static void send_stream_to_vlc(struct vlc_info vlc_info, unsigned char *data, int size)
{
    int ret, sendbytes = 0;
    struct sockaddr_in vlc_addr;

    if (vlc_info.server_sock != -1)
    {
        bzero(&vlc_addr, sizeof(struct sockaddr_in));
        vlc_addr.sin_family      = AF_INET;
        vlc_addr.sin_addr.s_addr = inet_addr(vlc_info.ip);
        vlc_addr.sin_port        = htons(vlc_info.port);

        while (size > 0)
        {
            int tosend = 1024;
            if (size < tosend)
                tosend = size;

            ret = sendto(vlc_info.server_sock, (char *)data + sendbytes, tosend, 0, (struct sockaddr *)&vlc_addr,
                         sizeof(struct sockaddr_in));

            if (ret < 0)
            {
                printf("[ERROR]: send(%d, %d) failed,%d,%s\n", vlc_info.server_sock, tosend, ret, strerror(errno));
            }
            else
            {
                sendbytes += ret;
                size -= ret;
            }
        }
    }
}

static void send_thread_proc(void *arg)
{
    struct rec_pool_info *pool;
    int has_data;
    int r, w;
    struct vlc_info *vlc_info = (struct vlc_info *)arg;

    if (vlc_info->server_sock == -1)
    {
        vlc_info->server_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        if (vlc_info->server_sock == -1)
        {
            perror("[ERROR]: create stream server socket\n");
            return;
        }
    }

    while (!g_cancel)
    {
        has_data = 0;
        pool     = g_vlcview_info.rec_pool + vlc_info->channel;

        rt_mutex_take(&vlc_info->rec_pool_lock, RT_WAITING_FOREVER);
        r = pool->read_index;
        w = pool->write_index;
        if (r != w)
        {
            if (w > r)
            {
                send_stream_to_vlc(*vlc_info, pool->addr + r, w - r);
            }
            else
            {
                send_stream_to_vlc(*vlc_info, pool->addr + r, pool->total_len - r);
                send_stream_to_vlc(*vlc_info, pool->addr, w);
            }
            has_data         = 1;
            pool->read_index = w;
        }
        rt_mutex_release(&vlc_info->rec_pool_lock);

        if (has_data == 0)
        {
            rt_thread_delay(1);
        }
    }
    return;
}

static int init_rec_pool(void)
{
    int i;
    unsigned char *p;

    g_vlcview_info.channel_count = MAX_ENC_CHAN;
    p = g_rec_pool_buf;

    for (i = 0; i < g_vlcview_info.channel_count; i++)
    {
        if (i == 0)
            g_vlcview_info.rec_pool[i].total_len = 1024 * 1024;
        else
            g_vlcview_info.rec_pool[i].total_len = 512 * 1024;
        g_vlcview_info.rec_pool[i].write_index = 0;
        g_vlcview_info.rec_pool[i].read_index  = 0;
        g_vlcview_info.rec_pool[i].addr = p;
        p += g_vlcview_info.rec_pool[i].total_len;
    }

    if (p > g_rec_pool_buf + DSP_SHARE_BUF_LEN)
    {
        printf("Error: rec pool address out of buf! need %d bytes, but have %d bytes\n", p - g_rec_pool_buf,
               DSP_SHARE_BUF_LEN);
        return -1;
    }

    return 0;
}

int vlcview_pes_init(void)
{
    int ret;
    int i;

    g_cancel = 0;
    ret      = init_rec_pool();
    if (ret != 0)
        return ret;
    ret = init_stream_pack();
    if (ret != 0)
        return ret;

    for (i = 0; i < MAX_ENC_CHAN; i++)
    {
        g_vlcview_info.vlc_info[i].channel     = -1;
        g_vlcview_info.vlc_info[i].server_sock = -1;
        g_vlcview_info.vlc_info[i].port        = 0;
        g_vlcview_info.vlc_info[i].thread_id   = 0;
        rt_mutex_init(&g_vlcview_info.vlc_info[i].rec_pool_lock, "app_mutex", RT_IPC_FLAG_PRIO);
    }

    return 0;
}

int vlcview_pes_uninit(void)
{
    int i;

    g_cancel = 1;

    for (i = 0; i < MAX_ENC_CHAN; i++)
    {
        if (g_vlcview_info.vlc_info[i].server_sock != -1)
            lwip_close(g_vlcview_info.vlc_info[i].server_sock);
        if (g_vlcview_info.vlc_info[i].thread_id != 0)
            rt_thread_delete(g_vlcview_info.vlc_info[i].thread_id);
    }

    return 0;
}

int vlcview_pes_send_to_vlc(int channel, const char *ip, int port)
{
    char thread_name[20];

    g_vlcview_info.vlc_info[channel].channel = channel;
    strcpy(g_vlcview_info.vlc_info[channel].ip, ip);
    g_vlcview_info.vlc_info[channel].port = port;

    snprintf(thread_name, 20, "pes_to_vlc_chn_%d", channel);
    g_vlcview_info.vlc_info[channel].thread_id = rt_thread_create(
        thread_name, send_thread_proc, &g_vlcview_info.vlc_info[channel], 4 * 1024, RT_APP_THREAD_PRIORITY, 10);

    rt_thread_startup(g_vlcview_info.vlc_info[channel].thread_id);

    return 0;
}

#include <string.h>
#include <stdio.h>
#include <rtthread.h>

#include "platform_def.h"
#include "isp_api.h"
#include "logo_64x64.h"
#include "multi_sensor.h"
#include "bufCtrl.h"
#include "FHAdv_video_cap.h"
#include "FHAdv_Isp_mpi.h"

#ifdef FH_USING_COOLVIEW
#include "dbi/dbi_over_tcp.h"
#define DBI_PORT 8888

static int g_exit = 1;
static rt_thread_t g_thread_dbg = RT_NULL;
#endif

#ifdef FH_USING_RTSP
#include "rtsp.h"
struct rtsp_server_context *g_rtsp_servers[2];
#else
#include "pes_pack.h"
#endif

#ifdef FH_USING_ADVAPI_MD
#include "FHAdv_MD_mpi.h"

struct mdcd_info
{
    rt_thread_t thread_id;
    FH_BOOL inited;
    FH_BOOL running;
};

static struct mdcd_info g_md_info    = {0};
static struct mdcd_info g_md_ex_info = {0};
static struct mdcd_info g_cd_info    = {0};
#endif

static FHADV_VIDEO_OSD_t g_osd_info  = {0};
FH_BOOL g_sdk_mem_inited = FH_FALSE;

extern int bufferInit(unsigned char* pAddr, unsigned int bufSize);

void sdk_mem_init(void)
{
    if (g_sdk_mem_inited)
        return;
    g_sdk_mem_inited = FH_TRUE;
    bufferInit((unsigned char *)FH_SDK_MEM_START, FH_SDK_MEM_SIZE);
}

static int sensor_init = 0;
int sdk_sensor_mem_init(void)
{
    FHADV_ISP_SENSOR_PROBE_INFO_t sensor_probe;

    if (!sensor_init)
    {
        sdk_mem_init();

        get_isp_sensor_info(&sensor_probe.sensor_infos, &sensor_probe.sensor_num);

        if (FHAdv_Isp_SensorInit(&sensor_probe, NULL) < 0)
            return -1;

        sensor_init = 1;
    }

    return 0;
}

unsigned char *find_start_code(unsigned char *pos, int len)
{
    unsigned char *p   = pos;
    unsigned char *end = pos + len;

    while (p != end)
    {
        if (p + 4 > end)
            break;
        if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 1)
            return p;
        else
            p += 1;
    }
    return NULL;
}

inline void send_data_for_channel(int chn, const FHADV_VIDEO_FRM_INFO_t *info, const FH_ADDR data, const FH_SINT32 len)
{
    int i     = 0;
    unsigned char *pos = data;
    unsigned char *end = data + len;
    int rest  = len;
    unsigned char *next_start;

#ifdef FH_USING_RTSP
    struct rtsp_server_context *server = g_rtsp_servers[chn];
#else
    struct vlcview_enc_stream_element stream_element;

    stream_element.frame_type = info->key_frame ? VLCVIEW_ENC_H264_I_FRAME : VLCVIEW_ENC_H264_P_FRAME;
    stream_element.frame_len  = len;
    stream_element.time_stamp = info->ts;
    stream_element.nalu_count = 0;
#endif

    do
    {
        next_start = find_start_code(pos + 4, rest - 4);
        if (next_start)
        {
            #ifdef FH_USING_RTSP
            rtp_push_data(server, pos, next_start - pos, info->ts);
            #else
            stream_element.nalu[i].start = pos;
            stream_element.nalu[i].len   = next_start - pos;
            stream_element.nalu_count++;
            #endif
            i++;
            rest = rest - (next_start - pos);
            pos  = next_start;
        }
        else
        { /* reach the end of data */
            #ifdef FH_USING_RTSP
            rtp_push_data(server, pos, end - pos, info->ts);
            #else
            stream_element.nalu[i].start = pos;
            stream_element.nalu[i].len   = end - pos;
            stream_element.nalu_count++;
            #endif
        }
    } while (next_start != NULL);

#ifndef FH_USING_RTSP
    vlcview_pes_stream_pack(chn, stream_element);
#endif
}

void video_input_callback_chn_0(const FHADV_VIDEO_FRM_INFO_t *info, const FH_ADDR data, const FH_SINT32 len)
{
    send_data_for_channel(0, info, data, len);
}

void video_input_callback_chn_1(const FHADV_VIDEO_FRM_INFO_t *info, const FH_ADDR data, const FH_SINT32 len)
{
    send_data_for_channel(1, info, data, len);
}

int vlcview(const char *ip, int port)
{
    FHADV_VIDEO_CHANNEL_t ch_main, ch_sub;
    FHADV_ISP_SENSOR_PROBE_INFO_t sensor_probe;

    if (ip == RT_NULL)
    {
        rt_kprintf("Error, target ip is empty\n");
        return -1;
    }

    sdk_mem_init();

    get_isp_sensor_info(&sensor_probe.sensor_infos, &sensor_probe.sensor_num);

    if (FHAdv_Isp_SensorInit(&sensor_probe, NULL) < 0)
        return -1;
    if (FHAdv_Video_Open() == FH_FAILURE)
        return -1;

    ch_main.channelId = 0;
    ch_main.res       = FHADV_VIDEO_RES_720P;
    ch_main.fps       = 25;
    ch_main.bitrate   = 1536;
    ch_main.gop       = 4 * ch_main.fps;
    ch_main.rcmode    = FHADV_RC_VBR;
    ch_main.cb        = video_input_callback_chn_0;
    if (FHAdv_Video_AddChannel(&ch_main) == FH_FAILURE)
        return -1;

    ch_sub.channelId = 1;
    ch_sub.res       = FHADV_VIDEO_RES_480P;
    ch_sub.fps       = 25;
    ch_sub.bitrate   = 512;
    ch_sub.gop       = 4 * ch_sub.fps;
    ch_sub.rcmode    = FHADV_RC_VBR;
    ch_sub.cb        = video_input_callback_chn_1;
    if (FHAdv_Video_AddChannel(&ch_sub) == FH_FAILURE)
        return -1;

    if (FHAdv_Video_Start(-1) == FH_FAILURE)
        return -1;

#ifdef FH_USING_RTSP
    enum rtp_transport transport;
#if FH_USING_RTSP_RTP_TCP
    transport = RTP_TRANSPORT_TCP;
#elif FH_USING_RTSP_RTP_UDP
    transport = RTP_TRANSPORT_UDP;
#endif
    g_rtsp_servers[0] = rtsp_start_server(transport, 1234);
    g_rtsp_servers[1] = rtsp_start_server(transport, 1236);
#else
    vlcview_pes_init();
    vlcview_pes_send_to_vlc(0, ip, port);
    vlcview_pes_send_to_vlc(1, ip, port + 1);
#endif

#ifdef FH_USING_COOLVIEW
    g_exit = 0;
    if (g_thread_dbg == RT_NULL)
    {
        struct dbi_tcp_config tcp_conf;
        tcp_conf.cancel = &g_exit;
        tcp_conf.port = DBI_PORT;

        g_thread_dbg = rt_thread_create("coolview", (void *)tcp_dbi_thread, &tcp_conf,
                                        4 * 1024, RT_APP_THREAD_PRIORITY + 10, 10);
        if (g_thread_dbg != RT_NULL)
        {
            rt_thread_startup(g_thread_dbg);
        }
    }
#endif
    return 0;
}

void vlcview_exit(void)
{
    g_sdk_mem_inited = FH_FALSE;
    FHAdv_Video_Stop(-1);
    FHAdv_Video_Close();
#ifdef FH_USING_RTSP
    rtsp_stop_server(g_rtsp_servers[0]);
    rtsp_stop_server(g_rtsp_servers[1]);
#else
    vlcview_pes_uninit();
#endif
}

void print_func_help(void)
{
    printf("Available function:\n");
    printf("   video_test(\"h\")                   print help info\n");
    printf("   video_test(\"r\", chn, angle)       rotate 0, 90, 180, 270 degree\n");
    printf("   video_test(\"i\", chn)              request I frame\n");
    printf("   video_test(\"j\", chn)              capture jpeg\n");
    printf("   video_test(\"y\", w, h)             capture yuv frame\n");
    printf("   video_test(\"b\", chn, bps, rcmode) change bitrate, bps in Kbps, rcmode: 0-CBR 1-VBR\n");
    printf("   video_test(\"d\")                   check day or night\n");
    printf("   video_test(\"g\")                   toggle show/off logo\n");
    printf("   video_test(\"t\")                   toggle show/off time osd\n");
    printf("   video_test(\"f\")                   toggle flip up/down\n");
    printf("   video_test(\"m\")                   toggle mirror left/right\n");
    printf("   video_test(\"c\", chn, new_res)     test remove and add channel to set new resolution\n");
    printf("   video_test(\"p\", chn)              toggle start/stop certain channel\n");
}

void test_rotate(int chn, int angle)
{
    FHAdv_Video_SetRotate(chn, angle);
}

void test_set_iframe(int chn)
{
    FHAdv_Video_ForceIFrame(chn);
}

static char test_jpeg_buf[100 * 1024];
void test_capture_jpeg(int chn)
{
    int len;

    if (FH_SUCCESS == FHAdv_Video_CaptureJpeg(chn, (FH_ADDR)test_jpeg_buf, &len))
    {
        rt_kprintf(
            "INFO: Capture JPEG picture successfully, "
            "the size is %d\n",
            len);
    }
}

int capture_jpeg(unsigned char *buf, int bufsz)
{
    int len = bufsz;

    FHADV_VIDEO_CHANNEL_t ch_main;

    sdk_sensor_mem_init();

    if (FHAdv_Video_Open() == FH_FAILURE)
        return -1;

    ch_main.channelId = 0;
    ch_main.res       = FHADV_VIDEO_RES_720P;
    ch_main.fps       = 25;
    ch_main.bitrate   = 1536;
    ch_main.gop       = 20;
    ch_main.rcmode    = FHADV_RC_CBR;
    ch_main.cb        = NULL;
    if (FHAdv_Video_AddChannel(&ch_main) == FH_FAILURE)
        return -1;

    if (FHAdv_Video_Start(-1) == FH_FAILURE)
        return -1;

    /* wait for 100ms */
    rt_thread_delay(rt_tick_from_millisecond(100));

    if (FH_SUCCESS == FHAdv_Video_CaptureJpeg(0, (FH_ADDR)buf, &len))
    {
        rt_kprintf(
            "INFO: Capture JPEG picture successfully, "
            "the size is %d\n",
            len);
    }
    else
    {
        len = 0;
    }

    FHAdv_Video_Stop(-1);
    FHAdv_Video_Close();

    return len;
}

int startup_mjpeg(void)
{
    FHADV_VIDEO_CHANNEL_t ch_main;

    sdk_sensor_mem_init();

    if (FHAdv_Video_Open() == FH_FAILURE)
        return -1;

    ch_main.channelId = 0;
    ch_main.res       = FHADV_VIDEO_RES_720P;
    ch_main.fps       = 15;
    ch_main.bitrate   = 1024;
    ch_main.gop       = 20;
    ch_main.rcmode    = FHADV_RC_CBR;
    ch_main.cb        = NULL;
    if (FHAdv_Video_AddChannel(&ch_main) == FH_FAILURE)
        return -1;

    if (FHAdv_Video_Start(-1) == FH_FAILURE)
        return -1;

    return 0;
}

static char test_yuv_buf[1280 * 720 * 3 / 2];
void test_capture_yuv(int w, int h)
{
    int ret;

    rt_kprintf("\nTest getting yuv data:  ");
    ret = FHAdv_Video_CaptureYUV(w, h, (FH_ADDR)test_yuv_buf);
    rt_kprintf("%s\n\n", ret == FH_SUCCESS ? "OK" : "FAIL");
}

void test_change_bitrate(int chn, int bps, int rcmode)
{
    FHAdv_Video_SetRateControl(chn, bps, rcmode);
}

void test_day_night(void)
{
    int status;

    status = FHAdv_Video_GetDayNight();
    if (FHADV_VIDEO_UNKNOWN == status)
    {
        rt_kprintf("Error: unknow status\n");
        return;
    }
    rt_kprintf("INFO: current is %s\n", status == FHADV_VIDEO_NIGHT ? "night" : "day");
}

void test_toggle_logo(void)
{
    g_osd_info.buf   = logo_data;
    g_osd_info.pic_w = AUTO_GEN_PIC_WIDTH;
    g_osd_info.pic_h = AUTO_GEN_PIC_HEIGHT;

    g_osd_info.pic_enable = !g_osd_info.pic_enable;
    g_osd_info.pic_x      = 1270 - g_osd_info.pic_w;
    g_osd_info.pic_y      = 10;

    FHAdv_Video_SetOSD(0, &g_osd_info);
}

void test_toggle_time_osd(void)
{
    g_osd_info.time_enable = !g_osd_info.time_enable;
    g_osd_info.time_x      = 10;
    g_osd_info.time_y      = 10;

    FHAdv_Video_SetOSD(0, &g_osd_info);
}

void test_toggle_flip(void)
{
    static int enable = FH_FALSE;

    enable = !enable;
    FHAdv_Video_SetFlipMirror(enable, FH_FALSE);
}

void test_toggle_mirror(void)
{
    static int enable = FH_FALSE;

    enable = !enable;
    FHAdv_Video_SetFlipMirror(FH_FALSE, enable);
}

void test_rm_chn(int chn, int new_res)
{
    FHADV_VIDEO_CHANNEL_t chn_info;

    if (new_res <= 0 || new_res > 3)
    {
        rt_kprintf("Error: new_res must be either of: 1->720p 2->480p 3->360p");
        return;
    }

    FHAdv_Video_RemoveChannel(chn);
    chn_info.channelId = chn;
    chn_info.res       = new_res;
    chn_info.fps       = 25;
    chn_info.bitrate   = 1536;
    chn_info.gop       = 20;
    chn_info.rcmode    = FHADV_RC_CBR;
    if (chn == 0)
        chn_info.cb = video_input_callback_chn_0;
    else
        chn_info.cb = video_input_callback_chn_1;
    FHAdv_Video_AddChannel(&chn_info);
    FHAdv_Video_Start(chn);
}

void test_stop_start_chn(int chn)
{
    static FH_BOOL stopped = FH_FALSE;

    stopped = !stopped;
    if (stopped)
        FHAdv_Video_Stop(chn);
    else
        FHAdv_Video_Start(chn);
}

void video_test(char *cmd, ...)
{
    va_list args;

    va_start(args, cmd);

    if (strcmp(cmd, "h") == 0)
    {
        print_func_help();
    }
    else if (strcmp(cmd, "r") == 0)
    {
        int chn, angle;

        chn   = va_arg(args, int);
        angle = va_arg(args, int);
        test_rotate(chn, angle);
    }
    else if (strcmp(cmd, "i") == 0)
    {
        int chn = va_arg(args, int);
        test_set_iframe(chn);
    }
    else if (strcmp(cmd, "j") == 0)
    {
        int chn = va_arg(args, int);
        test_capture_jpeg(chn);
    }
    else if (strcmp(cmd, "y") == 0)
    {
        int w, h;

        w = va_arg(args, int);
        h = va_arg(args, int);
        test_capture_yuv(w, h);
    }
    else if (strcmp(cmd, "b") == 0)
    {
        int chn, bps, rcmode;

        chn    = va_arg(args, int);
        bps    = va_arg(args, int);
        rcmode = va_arg(args, int);
        test_change_bitrate(chn, bps, rcmode);
    }
    else if (strcmp(cmd, "d") == 0)
    {
        test_day_night();
    }
    else if (strcmp(cmd, "g") == 0)
    {
        test_toggle_logo();
    }
    else if (strcmp(cmd, "t") == 0)
    {
        test_toggle_time_osd();
    }
    else if (strcmp(cmd, "f") == 0)
    {
        test_toggle_flip();
    }
    else if (strcmp(cmd, "m") == 0)
    {
        test_toggle_mirror();
    }
    else if (strcmp(cmd, "c") == 0)
    {
        int chn, new_res;

        chn     = va_arg(args, int);
        new_res = va_arg(args, int);
        test_rm_chn(chn, new_res);
    }
    else if (strcmp(cmd, "p") == 0)
    {
        int chn;

        chn = va_arg(args, int);
        test_stop_start_chn(chn);
    }
    else
    {
        rt_kprintf("Invalid function cmd !\n");
        print_func_help();
    }
    va_end(args);
}

#ifdef FH_USING_ADVAPI_MD
void md_task(void *arg)
{
    FH_SINT32 ret;
    FH_UINT32 total_result;

    while (g_md_info.running)
    {
        FHAdv_MD_CD_Check();

        ret = FHAdv_MD_GetResult(&total_result);
        if (ret == FH_SUCCESS)
        {
            printf("\rMD result: %s", total_result != 0 ? "YES" : "NO ");
            fflush(stdout);
        }
        else
        {
            printf("\nError: FHAdv_MD_GetResult failed\n");
        }
        rt_thread_delay(rt_tick_from_millisecond(25));
    }
}

void md_start(void)
{
    FHT_MDConfig_t md_config;
    FH_SINT32 ret;

    if (g_md_info.inited == FH_FALSE)
    {
        g_md_info.inited = FH_TRUE;
        ret = FHAdv_MD_Init();
        if (ret != FH_SUCCESS)
        {
            printf("Error: FHAdv_MD_Init failed\n");
            return;
        }
    }

    g_md_info.running = FH_TRUE;

    memset(&md_config, 0, sizeof(FHT_MDConfig_t));

    md_config.enable               = FH_TRUE;
    md_config.threshold            = 150;
    md_config.framedelay           = 3;
    md_config.md_area[0].fTopLeftX = 0;
    md_config.md_area[0].fTopLeftY = 0;
    md_config.md_area[0].fWidth    = 100;
    md_config.md_area[0].fHeigh    = 100;

    ret = FHAdv_MD_SetConfig(&md_config);
    if (ret != FH_SUCCESS)
    {
        printf("Error: FHAdv_MD_SetConfig failed\n");
        return;
    }

    g_md_info.thread_id = rt_thread_create("md_task", md_task, NULL,
                                           4 * 1024, RT_APP_THREAD_PRIORITY + 20, 10);
    if (g_md_info.thread_id != RT_NULL)
    {
        rt_thread_startup(g_md_info.thread_id);
    }
}

void md_stop(void)
{
    g_md_info.running = FH_FALSE;
    if (g_md_info.thread_id != RT_NULL)
        rt_thread_detach(g_md_info.thread_id);
}

void md_ex_task(void *arg)
{
    FH_SINT32 ret;
    FH_SINT32 count = 0;
    FH_SINT32 i, j/*, k*/;
    FHT_MDConfig_Ex_Result_t result;
    FH_UINT8 block_result;

    while (g_md_ex_info.running)
    {
        count++;

        FHAdv_MD_CD_Check();
        ret = FHAdv_MD_Ex_GetResult(&result);
        if (ret == FH_SUCCESS)
        {
            if ((count % 50) == 0)
            {
                count = 0;
                printf("\033[H\033[J");
                printf("======================= Motion Detect Result  ===========================\n");
                for (i = 0; i < result.vertical_count; i++)
                {
                    for (j = 0; j < result.horizontal_count; j++)
                    {
                        block_result = *(result.start + i * result.horizontal_count + j);
                        if (block_result > 0)
                            printf("o ");
                        else
                            printf("  ");
                    }
                    printf("\n");
                }
            }
        }
        else
        {
            printf("\nError: FHAdv_MD_Ex_GetResult failed, ret=%d\n", ret);
        }
        rt_thread_delay(rt_tick_from_millisecond(25));
    }
}

void md_ex_start(void)
{
    FHT_MDConfig_Ex_t md_ex_config;
    FH_SINT32 ret;

    if (g_md_ex_info.inited == FH_FALSE)
    {
        g_md_ex_info.inited = FH_TRUE;
        ret = FHAdv_MD_Ex_Init();
        if (ret != FH_SUCCESS)
        {
            printf("Error: FHAdv_MD_Ex_Init failed: %d\n", ret);
            return;
        }
    }

    g_md_ex_info.running = FH_TRUE;

    md_ex_config.threshold  = 80;
    md_ex_config.framedelay = 3;
    md_ex_config.enable     = FH_TRUE;

    ret = FHAdv_MD_Ex_SetConfig(&md_ex_config);
    if (ret != FH_SUCCESS)
    {
        printf("Error: FHAdv_MD_Ex_SetConfig failed\n");
        return;
    }

    g_md_ex_info.thread_id = rt_thread_create("md_ex_task", md_ex_task, NULL,
                                               4 * 1024, RT_APP_THREAD_PRIORITY + 20, 10);
    if (g_md_ex_info.thread_id != RT_NULL)
    {
        rt_thread_startup(g_md_ex_info.thread_id);
    }
}

void md_ex_stop(void)
{
    g_md_ex_info.running = FH_FALSE;
    if (g_md_ex_info.thread_id != RT_NULL)
        rt_thread_detach(g_md_ex_info.thread_id);
}

void cd_task(void *arg)
{
    FH_SINT32 ret;
    FH_UINT32 result;

    while (g_cd_info.running)
    {
        FHAdv_MD_CD_Check();

        ret = FHAdv_CD_GetResult(&result);
        if (ret == FH_SUCCESS)
        {
            printf("\rCD result: %s", result != 0 ? "YES" : "NO ");
            fflush(stdout);
        }
        else
        {
            printf("\nError: FHAdv_CD_GetResult failed, ret=%d\n", ret);
        }
        rt_thread_delay(rt_tick_from_millisecond(25));
    }
}

void cd_start(void)
{
    FHT_CDConfig_t cd_config;
    FH_SINT32 ret;

    if (g_cd_info.inited == FH_FALSE)
    {
        g_cd_info.inited = FH_TRUE;
        ret = FHAdv_CD_Init();
        if (ret != FH_SUCCESS)
        {
            printf("Error: FHAdv_CD_Init failed\n");
            return;
        }
    }

    g_cd_info.running = FH_TRUE;

    cd_config.cd_level   = CD_LEVEL_MID;
    cd_config.framedelay = 3;
    cd_config.enable     = FH_TRUE;

    ret = FHAdv_CD_SetConfig(&cd_config);
    if (ret != FH_SUCCESS)
    {
        printf("Error: FHAdv_CD_SetConfig failed\n");
        return;
    }

    g_cd_info.thread_id = rt_thread_create("cover_detect_task", cd_task, NULL,
                                           4 * 1024, RT_APP_THREAD_PRIORITY + 20, 10);
    if (g_cd_info.thread_id != RT_NULL)
    {
        rt_thread_startup(g_cd_info.thread_id);
    }
}

void cd_stop(void)
{
    g_cd_info.running = FH_FALSE;
    if (g_cd_info.thread_id != RT_NULL)
        rt_thread_detach(g_cd_info.thread_id);
}
#endif // FH_USING_ADVAPI_MD

#ifdef FH_USING_ADVAPI_ISP
void test_isp(int seq)
{
    if (seq == 0)
    {
        printf("\nUsage: test_isp(test_sequence_number)\n"
               "\ntest_sequence_number:\n"
               "         1  test get/set AWB gain\n"
               "         2  test get/set AE mode\n"
               "         3  test get/set LTM config\n"
               "         4  test get/set brightness\n"
               "         5  test get/set contrast\n"
               "         6  test get/set saturation\n"
               "         7  test get/set sharpness\n"
               "         8  test enable/disable mirror or flip\n"
               "         9  test get/set color mode\n\n");
        return;
    }

    FHAdv_Isp_Init();

    if (seq == 1)
    {
        FH_UINT32 awbgain[4];
        int i;

        FHAdv_Isp_GetAwbGain(awbgain);
        printf("original awb gain: %u %u %u %u\n", awbgain[0], awbgain[1], awbgain[2], awbgain[3]);

        for (i = 0; i < 6; i++)
        {
            printf("set awb gain mode %d\n", i);
            FHAdv_Isp_SetAwbGain(i);
            FHAdv_Isp_GetAwbGain(awbgain);
            printf("mode %d   awb gain: %u %u %u %u\n\n", i, awbgain[0], awbgain[1], awbgain[2], awbgain[3]);
            rt_thread_delay(200);
        }
    }
    else if (seq == 2)
    {
        FH_UINT32 ae_lines, gain_value;
        int i;

        FHAdv_Isp_GetAEMode(&ae_lines, &gain_value);
        printf("get ae mode: ae_lines=%u gain_value=%u\n", ae_lines, gain_value);

        for (i = 0; i < 4; i++)
        {
            printf("set ae mode %d with ae_time=1/3 gain_level=50\n", i);
            FHAdv_Isp_SetAEMode(i, FH_SINT32t_1_3, 50);
            rt_thread_delay(200);
        }
    }
    else if (seq == 3)
    {
        FH_SINT32 mode, level;

        FHAdv_Isp_GetLtmCfg(&mode, &level);
        printf("original ltm cfg: mode=%d level=%d\n", mode, level);

        printf("set mode=0 level=%d\n", level);
        FHAdv_Isp_SetLtmCfg(0, level);
        FHAdv_Isp_GetLtmCfg(&mode, &level);
        printf("get ltm cfg: mode=%d level=%d\n", mode, level);
        rt_thread_delay(200);
        printf("set mode=1 level=%d\n", level);
        FHAdv_Isp_SetLtmCfg(1, level);
        FHAdv_Isp_GetLtmCfg(&mode, &level);
        printf("get ltm cfg: mode=%d level=%d\n", mode, level);
    }
    else if (seq == 4)
    {
        unsigned int value;

        FHAdv_Isp_GetBrightness(&value);
        printf("original brightness: %u\n", value);
        rt_thread_delay(200);
        printf("set new brightness:  %u\n", value + 50);
        FHAdv_Isp_SetBrightness(value + 50);
        FHAdv_Isp_GetBrightness(&value);
        printf("get brightness:      %u\n", value);
    }
    else if (seq == 5)
    {
        int mode;
        unsigned int value;

        FHAdv_Isp_GetContrast(&mode, &value);
        printf("original contrast: mode=%d value=%u\n", mode, value);
        rt_thread_delay(200);
        printf("set mode=%d value=%u\n", mode, value + 50);
        FHAdv_Isp_SetContrast(mode, value + 50);
        FHAdv_Isp_GetContrast(&mode, &value);
        printf("get contrast: mode=%d value=%u\n", mode, value);
    }
    else if (seq == 6)
    {
        int mode;
        unsigned int value;

        FHAdv_Isp_GetSaturation(&mode, &value);
        printf("original saturation: mode=%d value=%u\n", mode, value);
        rt_thread_delay(200);
        printf("set mode=%d value=%u\n", mode, value + 50);
        FHAdv_Isp_SetSaturation(mode, value + 50);
        FHAdv_Isp_GetSaturation(&mode, &value);
        printf("get saturation: mode=%d value=%u\n", mode, value);
    }
    else if (seq == 7)
    {
        int mode;
        unsigned int value;

        FHAdv_Isp_GetSharpeness(&mode, &value);
        printf("original sharpness: mode=%d value=%u\n", mode, value);
        rt_thread_delay(200);
        printf("set mode=%d value=%u\n", mode, value + 10);
        FHAdv_Isp_SetSharpeness(mode, value + 10);
        FHAdv_Isp_GetSharpeness(&mode, &value);
        printf("get sharpness: mode=%d value=%u\n", mode, value);
    }
    else if (seq == 8)
    {
        FH_SINT32 mirror, flip;

        FHAdv_Isp_GetMirrorAndflip(&mirror, &flip);
        printf("get mirror=%d flip=%d\n", mirror, flip);

        printf("set mirror=%d flip=%d\n", !mirror, flip);
        FHAdv_Isp_SetMirrorAndflip(!mirror, flip);
        FHAdv_Isp_GetMirrorAndflip(&mirror, &flip);
        printf("get mirror=%d flip=%d\n", mirror, flip);
        rt_thread_delay(200);
        printf("set mirror=%d flip=%d\n", mirror, !flip);
        FHAdv_Isp_SetMirrorAndflip(mirror, !flip);
        FHAdv_Isp_GetMirrorAndflip(&mirror, &flip);
        printf("get mirror=%d flip=%d\n", mirror, flip);
    }
    else if (seq == 9)
    {
        int mode;

        FHAdv_Isp_GetColorMode(&mode);
        printf("original color mode: %d\n", mode);

        printf("set color mode:      0\n");
        FHAdv_Isp_SetColorMode(0);
        FHAdv_Isp_GetColorMode(&mode);
        printf("get color mode:      %d\n", mode);
        rt_thread_delay(200);
        printf("set color mode:      1\n");
        FHAdv_Isp_SetColorMode(1);
        FHAdv_Isp_GetColorMode(&mode);
        printf("get color mode:      %d\n", mode);
    }
}
#endif // FH_USING_ADVAPI_ISP

#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(vlcview, vlcview(ip, port));
FINSH_FUNCTION_EXPORT(vlcview_exit, vlcview_exit());
FINSH_FUNCTION_EXPORT(video_test, use video_test("h") for help info);
#ifdef FH_USING_ADVAPI_MD
FINSH_FUNCTION_EXPORT(md_start, start motion detect thread);
FINSH_FUNCTION_EXPORT(md_stop, stop motion detect);
FINSH_FUNCTION_EXPORT(md_ex_start, start motion detect thread using ex API);
FINSH_FUNCTION_EXPORT(md_ex_stop, stop motion detect using ex API);
FINSH_FUNCTION_EXPORT(cd_start, start cover detect thread);
FINSH_FUNCTION_EXPORT(cd_stop, stop cover detect);
#endif // FH_USING_ADVAPI_MD
#ifdef FH_USING_ADVAPI_ISP
FINSH_FUNCTION_EXPORT(test_isp, test advapi isp functions);
#endif
#endif

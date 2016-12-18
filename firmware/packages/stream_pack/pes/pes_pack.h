
#ifndef __LIBVLCVIEW_H__
#define __LIBVLCVIEW_H__

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NALU_COUNT 20

enum vlcview_enc_frame_type
{
    VLCVIEW_ENC_H264_I_FRAME,
    VLCVIEW_ENC_H264_P_FRAME
};

struct vlcview_h264_nalu
{
    unsigned int len;
    unsigned char *start;
};

struct vlcview_enc_stream_element
{
    enum vlcview_enc_frame_type frame_type;
    int frame_len;
    int nalu_count;
    unsigned long long time_stamp;
    struct vlcview_h264_nalu nalu[MAX_NALU_COUNT];
};

int vlcview_pes_init(void);
int vlcview_pes_uninit(void);
int vlcview_pes_stream_pack(int channel, struct vlcview_enc_stream_element stream_element);
int vlcview_pes_send_to_vlc(int channel, const char *ip, int port);
int init_stream_pack();
int deinit_stream_pack();

#ifdef __cplusplus
}
#endif

#endif

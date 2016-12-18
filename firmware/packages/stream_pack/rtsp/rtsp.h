#ifndef __RTSP_H__
#define __RTSP_H__

#define RTSP_DEBUG

#ifdef RTSP_DEBUG
#define debug printf
#else
#define debug
#endif

enum rtp_transport
{
    RTP_TRANSPORT_UDP,
    RTP_TRANSPORT_TCP
};

struct rtsp_server_context;

struct rtsp_server_context *rtsp_start_server(enum rtp_transport transport, int port);
void rtsp_stop_server(struct rtsp_server_context *server);
void rtp_push_data(struct rtsp_server_context *server, void *data, int size, unsigned long long pts);

#endif

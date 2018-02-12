#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <errno.h>
#include <sys/time.h>
#include <rtthread.h>

#include <netdb.h>
#include <sys/socket.h>
#include <lwip/netif.h>

#include "rtsp.h"

/* rtp packet macros */
#ifndef TCP_MSS
#define TCP_MSS 1460
#endif
#define RTP_PACKET_MAX_SIZE 1472

/* rtsp macros */
#ifdef RT_LWIP_TCP_PCB_NUM
#define MAX_CLIENT_COUNT RT_LWIP_TCP_PCB_NUM
#else
#define MAX_CLIENT_COUNT 10
#endif
#define REQUEST_READ_BUF_SIZE 256
#define RESPONSE_BUF_SIZE 1024
#define RTP_UDP_SEND_PORT 3056
#define RTSP_THREAD_PRIORITY (RT_THREAD_PRIORITY_MAX >> 1)
#define RTSP_THREAD_STACK_SIZE 1024
#define RTSP_THREAD_TIMESLICE 10

#define rtp_list_container_of(ptr, sample, member) \
    (__typeof__(sample))((char *)(ptr)-offsetof(__typeof__(*sample), member))

#define rtp_list_for_each(pos, head, member)                                             \
    for (pos = rtp_list_container_of((head)->next, pos, member); &pos->member != (head); \
         pos = rtp_list_container_of(pos->member.next, pos, member))

#define rtp_list_for_each_safe(pos, tmp, head, member)                                         \
    for (pos                         = rtp_list_container_of((head)->next, pos, member),       \
        tmp                          = rtp_list_container_of((pos)->member.next, tmp, member); \
         &pos->member != (head); pos = tmp, tmp = rtp_list_container_of(pos->member.next, tmp, member))

#if !LWIP_POSIX_SOCKETS_IO_NAMES
#define close lwip_close
// extern int fcntl(int s, int cmd, int val);
#endif

enum
{
    RTSP_MSG_NULL,
    RTSP_MSG_OPTIONS,
    RTSP_MSG_DESCRIBE,
    RTSP_MSG_SETUP,
    RTSP_MSG_PLAY,
    RTSP_MSG_TEARDOWN
};

struct list_t
{
    struct list_t *prev;
    struct list_t *next;
};

struct sdp_info
{
    char sps[32];
    int sps_len;
    int has_sps;
    char pps[32];
    int pps_len;
    int has_pps;
    char content[1024];
    int len;
};

struct rtp_packet
{
    char buf[RTP_PACKET_MAX_SIZE];
    int len;
    int header_len;
    struct list_t link;
};

struct pkt_pool
{
    char *buf;
    int len;
    int pos;
};

struct rtp_info
{
    unsigned short sequence_num;
    unsigned long ssrc;
    struct sdp_info sdp;
    struct pkt_pool pool;
    enum rtp_transport transport;
    int udp_send_socket;
};

struct client_context
{
    struct rtsp_server_context *server;
    int fd;
    struct sockaddr_in addr;
    int udp_send_port;
    int start_play;
    int find_i_frame;
    void *session;
    int (*process_func)(struct client_context *ctx);
    rt_thread_t thread_id;
    struct list_t link;
};

struct rtsp_server_context
{
    int stop;
    int sd;
    rt_thread_t thread_id;
    int port;
    void (*init_client)(struct client_context *ctx);
    void (*release_client)(struct client_context *ctx);
    struct rtp_info rtp_info;
    struct rt_semaphore client_list_lock;
    struct list_t client_list;
};

struct rtsp_session
{
    struct client_context *ctx;
    char read_buf[REQUEST_READ_BUF_SIZE];
    char response_buf[RESPONSE_BUF_SIZE];
    char cseq[10];
    unsigned long session_num;
};

void rtp_list_init(struct list_t *list)
{
    list->prev = list;
    list->next = list;
}

void rtp_list_add(struct list_t *list, struct list_t *elm)
{
    elm->prev       = list;
    elm->next       = list->next;
    list->next      = elm;
    elm->next->prev = elm;
}

void rtp_list_del(struct list_t *elm)
{
    elm->prev->next = elm->next;
    elm->next->prev = elm->prev;
    elm->next       = NULL;
    elm->prev       = NULL;
}

void init_packet_pool(struct rtp_info *info)
{
    info->pool.buf = malloc(512 * 1024);
    info->pool.len = 512 * 1024;
    info->pool.pos = 0;
}

void reset_packet_pool(struct rtp_info *info)
{
    info->pool.pos = 0;
}

struct rtp_packet *get_packet_from_pool(struct rtp_info *info)
{
    char *ptr;
    struct rtp_packet *pkt;

    ptr = info->pool.buf + info->pool.pos;
    info->pool.pos += sizeof(struct rtp_packet);
    memset(ptr, 0, sizeof(struct rtp_packet));
    pkt = (struct rtp_packet *)ptr;

    return pkt;
}

static void base64_encode(char *dst, char *src, int len)
{
    int out_len;
    int i;
    int left_count;
    int index;
    static const char base64[128] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    if ((left_count = len % 3) > 0)
        out_len = (len / 3 + 1) * 4;
    else
        out_len = len / 3 * 4;

    for (i = 0; i < len / 3; i++)
    {
        index          = src[i * 3] >> 2;
        dst[i * 4]     = base64[index];
        index          = ((src[i * 3] & 0x03) << 4) | ((src[i * 3 + 1] & 0xf0) >> 4);
        dst[i * 4 + 1] = base64[index];
        index          = ((src[i * 3 + 1] & 0x0f) << 2) | ((src[i * 3 + 2] & 0xc0) >> 6);
        dst[i * 4 + 2] = base64[index];
        index          = src[i * 3 + 2] & 0x3f;
        dst[i * 4 + 3] = base64[index];
    }
    if (left_count == 1)
    {
        index          = src[i * 3] >> 2;
        dst[i * 4]     = base64[index];
        index          = (src[i * 3] & 0x03) << 4;
        dst[i * 4 + 1] = base64[index];
        dst[i * 4 + 2] = '=';
        dst[i * 4 + 3] = '=';
    }
    if (left_count == 2)
    {
        index          = src[i * 3] >> 2;
        dst[i * 4]     = base64[index];
        index          = ((src[i * 3] & 0x03) << 4) | ((src[i * 3 + 1] & 0xf0) >> 4);
        dst[i * 4 + 1] = base64[index];
        index          = (src[i * +1] & 0x0f) << 2;
        dst[i * 4 + 2] = base64[index];
        dst[i * 4 + 3] = '=';
    }
    dst[out_len] = 0;
}

void generate_sdp(struct sdp_info *sdp)
{
    char tmp[64];

    if (!sdp->has_sps || !sdp->has_pps)
        return;

    sdp->content[0] = 0;

    strcat(sdp->content, "v=0\r\n");

    strcat(sdp->content, "m=video 0 RTP/AVP 96\r\n");
    strcat(sdp->content,
           "a=rtpmap:96 H264/90000\r\n"
           "a=fmtp:96 profile-level-id=4DE028;packetization-mode=1;"
           "sprop-parameter-sets=");

    base64_encode(tmp, sdp->sps, sdp->sps_len);
    strcat(sdp->content, tmp);
    strcat(sdp->content, ",");
    base64_encode(tmp, sdp->pps, sdp->pps_len);
    strcat(sdp->content, tmp);
    strcat(sdp->content, "\r\n");
    sdp->len = strlen(sdp->content);
}

int generate_rtp_header(char *dst, int payload_len, int is_last, unsigned short seq_num, unsigned long long pts,
                        unsigned long ssrc, enum rtp_transport transport)
{
    int index = 0;

    if (transport == RTP_TRANSPORT_TCP)
    {
        /* RTSP interleaved frame header */
        dst[index++] = 0x24;
        dst[index++] = 0x00;
        dst[index++] = ((payload_len + 12) >> 8) & 0xff;
        dst[index++] = (payload_len + 12) & 0xff;
    }

    /* RTP header */
    dst[index++] = 0x80;
    dst[index++] = (is_last ? 0x80 : 0x00) | 96;
    dst[index++] = (seq_num >> 8) & 0xff;
    dst[index++] = seq_num & 0xff;
    dst[index++] = (pts >> 24) & 0xff;
    dst[index++] = (pts >> 16) & 0xff;
    dst[index++] = (pts >> 8) & 0xff;
    dst[index++] = pts & 0xff;
    dst[index++] = (ssrc >> 24) & 0xff;
    dst[index++] = (ssrc >> 16) & 0xff;
    dst[index++] = (ssrc >> 8) & 0xff;
    dst[index++] = ssrc & 0xff;

    return index;
}

int _send(int fd, void *buf, int len)
{
    struct timeval tv;
    fd_set fds;
    int ret;

    do
    {
        tv.tv_sec  = 0;
        tv.tv_usec = 1000;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        ret = select(fd + 1, NULL, &fds, NULL, &tv);
        if (ret > 0)
        {
            if (FD_ISSET(fd, &fds))
            {
                send(fd, buf, len, 0);
            }
        }
        else if (ret == 0)
        {
            continue;
        }
        else
        {
            return -1;
        }
    } while (ret == 0);

    return 0;
}

void send_stream(struct client_context *client, struct rtp_packet *pkt)
{
    if (client->server->rtp_info.transport == RTP_TRANSPORT_UDP)
    {
        client->addr.sin_port = htons(client->udp_send_port);
        sendto(client->server->rtp_info.udp_send_socket, pkt->buf, pkt->len, 0, (struct sockaddr *)&client->addr,
               sizeof(struct sockaddr_in));
    }
    else
    {
        _send(client->fd, pkt->buf, pkt->len);
    }
}

void send_to_clients(struct rtsp_server_context *server, struct rtp_packet *pkt)
{
    struct client_context *client, *next;
    char *p = pkt->buf + pkt->header_len;

    rt_sem_take(&server->client_list_lock, RT_WAITING_FOREVER);
    rtp_list_for_each_safe(client, next, &server->client_list, link)
    {
        if (client)
        {
            if (client->start_play)
            {
                if (server->port != 0 && client->find_i_frame == 0)
                {                           /* using RTSP */
                    if ((p[4] & 0x1f) == 5) /* I frame */
                    {
                        client->find_i_frame = 1;
                    }
                }
                if (client->find_i_frame)
                {
                    send_stream(client, pkt);
                }
            }
        }
    }
    rt_sem_release(&server->client_list_lock);
}

void generate_rtp_packets_and_send(struct rtsp_server_context *server, struct list_t *pkt_list, void *data, int size,
                                   unsigned long long pts)
{
    char *nal;
    int nal_len;
    struct rtp_packet *pkt;
    int header_offset, max_len;

    if (pkt_list->next != pkt_list)
        return;

    if (server->rtp_info.transport == RTP_TRANSPORT_TCP)
    {
        header_offset = 16;
        max_len       = TCP_MSS;
    }
    else
    {
        header_offset = 12;
        max_len       = RTP_PACKET_MAX_SIZE;
    }

    nal     = data + 4;
    nal_len = size - 4;

    if (nal_len > max_len - header_offset)
    {
        int fragment_len = max_len - header_offset - 2;
        int pkt_num, i;
        int fu_len = fragment_len;
        char *fu_buf;

        if ((nal_len - 1) % fragment_len == 0)
            pkt_num = (nal_len - 1) / fragment_len;
        else
            pkt_num = (nal_len - 1) / fragment_len + 1;
        for (i = 0; i < pkt_num; i++)
        {
            pkt = get_packet_from_pool(&server->rtp_info);
            if (i == pkt_num - 1)
                fu_len = nal_len - 1 - i * fragment_len;
            server->rtp_info.sequence_num++;
            pkt->header_len = generate_rtp_header(pkt->buf, fu_len + 2, i == pkt_num - 1, server->rtp_info.sequence_num,
                                                  pts, server->rtp_info.ssrc, server->rtp_info.transport);
            fu_buf    = pkt->buf + header_offset;
            fu_buf[0] = 0x00 | (nal[0] & 0x60) | 28;
            fu_buf[1] = (i == 0 ? 0x80 : 0x00) | ((i == pkt_num - 1) ? 0x40 : 0x00) | (nal[0] & 0x1f);
            memcpy(fu_buf + 2, nal + 1 + i * fragment_len, fu_len);
            pkt->len = header_offset + 2 + fu_len;
            rtp_list_add(pkt_list->prev, &pkt->link);
            send_to_clients(server, pkt);
        }
    }
    else
    {
        server->rtp_info.sequence_num++;
        pkt             = get_packet_from_pool(&server->rtp_info);
        pkt->header_len = generate_rtp_header(pkt->buf, nal_len, 0, server->rtp_info.sequence_num, pts,
                                              server->rtp_info.ssrc, server->rtp_info.transport);
        memcpy(pkt->buf + header_offset, nal, nal_len);
        pkt->len = header_offset + nal_len;
        rtp_list_add(pkt_list->prev, &pkt->link);
        send_to_clients(server, pkt);
    }
}

void client_thread_proc(void *arg)
{
    struct client_context *client_ctx = (struct client_context *)arg;

    while (client_ctx->server->stop == 0)
    {
        if (client_ctx->process_func)
        {
            if (client_ctx->process_func(client_ctx) < 0)
            {
                rt_sem_take(&client_ctx->server->client_list_lock, RT_WAITING_FOREVER);
                rtp_list_del(&client_ctx->link);
                rt_sem_release(&client_ctx->server->client_list_lock);
                close(client_ctx->fd);
                if (client_ctx->server->release_client)
                    client_ctx->server->release_client(client_ctx);
                free(client_ctx);
                break;
            }
        }
    }
}

void rtp_tcp_server_thread(void *arg)
{
    struct rtsp_server_context *server_ctx = (struct rtsp_server_context *)arg;
    struct sockaddr_in addr;
    int on;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    struct client_context *client_ctx;
    int fd;
    char client_thread_name[32];

    if (rt_sem_init(&server_ctx->client_list_lock, "client_list_lock", 1, RT_IPC_FLAG_FIFO) != 0)
    {
        debug("Error: create mutex lock failed\n");
        return;
    }

    server_ctx->sd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_ctx->sd < 0)
    {
        debug("create server socket failed\n");
        return;
    }

    /* ignore "socket already in use" errors */
    on = 1;
    setsockopt(server_ctx->sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    fcntl(server_ctx->sd, F_SETFL, O_NONBLOCK);

    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(server_ctx->port);
    addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(addr.sin_zero), 8, sizeof(addr.sin_zero));

    if (bind(server_ctx->sd, (struct sockaddr *)&addr, sizeof(addr)) != 0)
    {
        debug("Unable to bind\n");
        return;
    }

    if (listen(server_ctx->sd, MAX_CLIENT_COUNT) != 0)
    {
        debug("Listen error\n");
        return;
    }

    while (!server_ctx->stop)
    {
        bzero(&addr, sizeof(addr));
        fd = accept(server_ctx->sd, (struct sockaddr *)&addr, &addr_len);
        if (fd != -1)
        {
            client_ctx = malloc(sizeof(struct client_context));
            if (client_ctx == NULL)
            {
                debug("failed to allocate (a very small amount of) memory\n");
                close(fd);
                continue;
            }
            memcpy(&client_ctx->addr, &addr, sizeof(addr));
            client_ctx->server       = server_ctx;
            client_ctx->fd           = fd;
            client_ctx->start_play   = 0;
            client_ctx->find_i_frame = 0;
            if (server_ctx->init_client)
                server_ctx->init_client(client_ctx);
            rtp_list_add(server_ctx->client_list.prev, &client_ctx->link);
            snprintf(client_thread_name, 32, "client_%d", client_ctx->fd);
            client_ctx->thread_id = rt_thread_create(client_thread_name, client_thread_proc, client_ctx,
                                                     RTSP_THREAD_STACK_SIZE, RTSP_THREAD_PRIORITY, RTSP_THREAD_TIMESLICE);
            if (client_ctx->thread_id != NULL)
                rt_thread_startup(client_ctx->thread_id);
        }
    }
    close(server_ctx->sd);
}

void rtp_push_data(struct rtsp_server_context *server, void *data, int size, unsigned long long pts)
{
    char *p = (char *)data;
    struct list_t packets;

    if (server == NULL)
        return;

    if (!server->rtp_info.sdp.has_sps || !server->rtp_info.sdp.has_pps)
    {
        if (7 == (p[4] & 0x1f))
        {
            memcpy(server->rtp_info.sdp.sps, p, size);
            server->rtp_info.sdp.sps_len = size;
            server->rtp_info.sdp.has_sps = 1;
        }
        else if (8 == (p[4] & 0x1f))
        {
            memcpy(server->rtp_info.sdp.pps, p, size);
            server->rtp_info.sdp.pps_len = size;
            server->rtp_info.sdp.has_pps = 1;
        }
        generate_sdp(&server->rtp_info.sdp);
    }

    if (server->client_list.next && server->client_list.next != &server->client_list)
    {
        rtp_list_init(&packets);
        generate_rtp_packets_and_send(server, &packets, data, size, pts * 9 / 100);
        reset_packet_pool(&server->rtp_info);
    }
}

void server_cleanup(struct rtsp_server_context *server)
{
    struct client_context *client, *next;

    if (server == NULL)
        return;

    rtp_list_for_each_safe(client, next, &server->client_list, link)
    {
        if (client)
        {
            if (client->thread_id != NULL)
                rt_thread_delete(client->thread_id);
            if (server->release_client)
                server->release_client(client);
            rtp_list_del(&client->link);
            if (client->fd != -1)
            {
                debug("close client socket %d\n", client->fd);
                close(client->fd);
            }
            free(client);
        }
    }
    if (server->sd != -1)
    {
        debug("close server socket %d\n", server->sd);
        close(server->sd);
    }
    if (server->rtp_info.udp_send_socket != -1)
    {
        debug("close udp send socket %d\n", server->rtp_info.udp_send_socket);
        close(server->rtp_info.udp_send_socket);
    }
    free(server->rtp_info.pool.buf);
    rt_sem_detach(&server->client_list_lock);
    free(server);
    server = NULL;
}

/**
 * rtsp requests processing functions
 */
void generate_response_header(struct rtsp_session *session)
{
    char tmp[32];

    session->response_buf[0] = 0;
    strcat(session->response_buf, "RTSP/1.0 200 OK\r\n");
    snprintf(tmp, sizeof(tmp), "CSeq: %s\r\n", session->cseq);
    strcat(session->response_buf, tmp);
}

void send_response(int fd, void *buffer, int size)
{
    send(fd, buffer, size, 0);
}

void generate_session_number(struct rtsp_session *session)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    session->session_num = tv.tv_sec * 1000L + tv.tv_usec / 1000L;
}

void rtsp_handle_options(struct rtsp_session *session)
{
    generate_response_header(session);
    strcat(session->response_buf, "Public: OPTIONS, DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE\r\n\r\n");
    send_response(session->ctx->fd, session->response_buf, strlen(session->response_buf));
}

void rtsp_handle_describe(struct rtsp_session *session)
{
    char tmp[32];

    generate_response_header(session);
    strcat(session->response_buf, "Content-Type: application/sdp\r\n");
    snprintf(tmp, sizeof(tmp), "Content-Length: %d\r\n\r\n", strlen(session->ctx->server->rtp_info.sdp.content));
    strcat(session->response_buf, tmp);
    strcat(session->response_buf, session->ctx->server->rtp_info.sdp.content);
    send_response(session->ctx->fd, session->response_buf, strlen(session->response_buf));
}

void rtsp_handle_setup(struct rtsp_session *session)
{
    char tmp[100];

    generate_session_number(session);
    generate_response_header(session);
    snprintf(tmp, sizeof(tmp), "Session: %lu\r\n", session->session_num);
    strcat(session->response_buf, tmp);

    if (session->ctx->server->rtp_info.transport == RTP_TRANSPORT_UDP)
    {
        if (session->ctx->udp_send_port == 0)
            session->ctx->udp_send_port = RTP_UDP_SEND_PORT;
        snprintf(tmp, 100,
                 "Transport: RTP/AVP;unicast;"
                 "client_port=%d-%d\r\n\r\n",
                 session->ctx->udp_send_port, session->ctx->udp_send_port + 1);
        strcat(session->response_buf, tmp);
    }
    else
    {
        strcat(session->response_buf,
               "Transport: RTP/AVP/TCP;unicast;interleaved=0-1;\r\n"
               "\r\n");
    }
    send_response(session->ctx->fd, session->response_buf, strlen(session->response_buf));
}

void rtsp_handle_play(struct rtsp_session *session)
{
    generate_response_header(session);
    strcat(session->response_buf, "\r\n");
    send_response(session->ctx->fd, session->response_buf, strlen(session->response_buf));
    session->ctx->start_play = 1;
}

int rtsp_get_request(struct rtsp_session *session)
{
    int msg_type;
    int ret;
    char *sub_str;
    fd_set fds;
    struct timeval tv;
    char *pos = NULL, *start = NULL;

    memset(session->read_buf, 0, REQUEST_READ_BUF_SIZE);

    FD_ZERO(&fds);
    FD_SET(session->ctx->fd, &fds);
    tv.tv_sec  = 0;
    tv.tv_usec = 10 * 1000;

    ret        = select(session->ctx->fd + 1, &fds, NULL, NULL, &tv);
    if (ret == 0)
        return RTSP_MSG_NULL;
    else if (ret < 0)
        return -1;

    if (FD_ISSET(session->ctx->fd, &fds))
    {
        ret = recv(session->ctx->fd, session->read_buf, REQUEST_READ_BUF_SIZE - 1, 0);
        if (ret < 0)
            return -1;
        else if (ret == 0)
            return RTSP_MSG_NULL;
        session->read_buf[REQUEST_READ_BUF_SIZE - 1] = 0;
    }

    if (strstr(session->read_buf, "OPTIONS"))
        msg_type = RTSP_MSG_OPTIONS;
    else if (strstr(session->read_buf, "DESCRIBE"))
        msg_type = RTSP_MSG_DESCRIBE;
    else if (strstr(session->read_buf, "SETUP"))
        msg_type = RTSP_MSG_SETUP;
    else if (strstr(session->read_buf, "PLAY"))
        msg_type = RTSP_MSG_PLAY;
    else if (strstr(session->read_buf, "TEARDOWN"))
        msg_type = RTSP_MSG_TEARDOWN;
    else
        msg_type = RTSP_MSG_NULL;

    pos = strstr(session->read_buf, "\r\n");
    if (pos)
    {
        start = pos + 2;
        if ((sub_str = strstr(start, "CSeq: ")) != NULL)
        {
            pos = strstr(sub_str, "\r\n");
            if (pos)
            {
                memcpy(session->cseq, sub_str + 6, pos - sub_str - 6);
                session->cseq[pos - sub_str - 6] = 0;
                start                            = pos + 2;
            }
        }
        if ((sub_str = strstr(start, "client_port=")) != NULL)
        {
            pos = strstr(sub_str, "-");
            if (pos)
            {
                char tmp[10];
                memcpy(tmp, sub_str + 12, pos - sub_str - 12);
                tmp[pos - sub_str - 12]     = 0;
                session->ctx->udp_send_port = strtol(tmp, NULL, 0);
            }
        }
    }

    return msg_type;
}

int rtsp_message_process(struct rtsp_session *session)
{
    int type;

    type = rtsp_get_request(session);

    if (type < 0)
        return -1;

    switch (type)
    {
    case RTSP_MSG_OPTIONS:
        rtsp_handle_options(session);
        break;
    case RTSP_MSG_DESCRIBE:
        rtsp_handle_describe(session);
        break;
    case RTSP_MSG_SETUP:
        rtsp_handle_setup(session);
        break;
    case RTSP_MSG_PLAY:
        rtsp_handle_play(session);
        break;
    case RTSP_MSG_TEARDOWN:
        session->ctx->start_play = 0;
        return -1;
    }
    return 0;
}

int rtsp_process_func(struct client_context *client)
{
    struct rtsp_session *session = (struct rtsp_session *)client->session;
    if (rtsp_message_process(session) < 0)
    {
        debug("rtsp: client teardown, socket=%d\n", client->fd);
        return -1;
    }
    return 0;
}

void init_rtsp_session(struct client_context *client)
{
    struct rtsp_session *session;

    debug("rtsp: new client connected, socket=%d\n", client->fd);

    session = malloc(sizeof(struct rtsp_session));
    memset(session, 0, sizeof(struct rtsp_session));
    session->ctx         = client;
    client->session      = session;
    client->process_func = rtsp_process_func;
}

void cleanup_rtsp_session(struct client_context *client)
{
    if (client->session)
        free(client->session);
}

struct rtsp_server_context *rtsp_start_server(enum rtp_transport transport, int port)
{
    struct rtsp_server_context *server;
    char thread_name[32];

    server = malloc(sizeof(struct rtsp_server_context));
    if (server == NULL)
    {
        debug("rtsp: failed to create server context object\n");
        return NULL;
    }
    memset(server, 0, sizeof(struct rtsp_server_context));

    rtp_list_init(&server->client_list);
    init_packet_pool(&server->rtp_info);

    if (transport != RTP_TRANSPORT_UDP && transport != RTP_TRANSPORT_TCP)
        server->rtp_info.transport = RTP_TRANSPORT_TCP;
    else
        server->rtp_info.transport = transport;

    if (server->rtp_info.transport == RTP_TRANSPORT_UDP)
    {
        server->rtp_info.udp_send_socket = socket(AF_INET, SOCK_DGRAM, 0);
        if (server->rtp_info.udp_send_socket == -1)
        {
            debug("rtsp: failed to create send socket\n");
            free(server);
            return NULL;
        }
    }

    server->stop           = 0;
    server->port           = port;
    server->init_client    = init_rtsp_session;
    server->release_client = cleanup_rtsp_session;
    
    snprintf(thread_name, 32, "rtsp_server:%d", port);
    server->thread_id = rt_thread_create(thread_name, rtp_tcp_server_thread, server, RTSP_THREAD_STACK_SIZE,
                                         RTSP_THREAD_PRIORITY, RTSP_THREAD_TIMESLICE);
    if (server->thread_id != NULL)
    {
        rt_thread_startup(server->thread_id);
    }
    else
    {
        debug("rtsp: failed to create rtsp server thread\n");
        free(server);
        return NULL;
    }

    return server;
}

void rtsp_stop_server(struct rtsp_server_context *server)
{
    debug("rtsp: stop server\n");

    server->stop = 1;

    if (server->thread_id != NULL)
    {
        debug("rtsp: delete server thread\n");
        rt_thread_delete(server->thread_id);
    }
    server_cleanup(server);
}

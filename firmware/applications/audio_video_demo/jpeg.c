#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <rtthread.h>
#include <finsh.h>

#include <dfs_posix.h>
#include <lwip/sockets.h>

#include <FHAdv_video_cap.h>

#define MJPEG_BOUNDARY "boundarydonotcross"

extern void startup_mjpeg(void);

static int  g_mjpeg_stop = 0;
static char g_send_buf[1024];

int send_first_response(int client)
{
    g_send_buf[0] = 0;

    rt_snprintf(g_send_buf, 1024,
             "HTTP/1.0 200 OK\r\n"
             "Connection: close\r\n"
             "Server: MJPG-Streamer/0.2\r\n"
             "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0,"
             " post-check=0, max-age=0\r\n"
             "Pragma: no-cache\r\n"
             "Expires: Mon, 3 Jan 2000 12:34:56 GMT\r\n"
             "Content-Type: multipart/x-mixed-replace;boundary="
             MJPEG_BOUNDARY "\r\n"
             "\r\n"
             "--" MJPEG_BOUNDARY "\r\n");
    if (send(client, g_send_buf, strlen(g_send_buf), 0) < 0)
    {
        lwip_close(client);
        return -1;
    }

    return 0;
}

int mjpeg_send_stream(int client, void *data, int size)
{
    g_send_buf[0] = 0;

    if (!g_mjpeg_stop)
    {
        snprintf(g_send_buf, 1024,
                 "Content-Type: image/jpeg\r\n"
                 "Content-Length: %d\r\n"
                 "\r\n", size);
        if (send(client, g_send_buf, strlen(g_send_buf), 0) < 0)
        {
            lwip_close(client);
            return -1;
        }

        if (send(client, data, size, 0) < 0)
        {
            lwip_close(client);
            return -1;
        }

        g_send_buf[0] = 0;
        snprintf(g_send_buf, 1024, "\r\n--" MJPEG_BOUNDARY "\r\n");
        if (send(client, g_send_buf, strlen(g_send_buf), 0) < 0)
        {
        	lwip_close(client);
        	return -1;
        }

        return 0;
    }

    return -1;
}

void mjpeg_server_thread(void *arg)
{
    int on;
    int srv_sock = -1;
    struct sockaddr_in addr;
    socklen_t sock_len = sizeof(struct sockaddr_in);

	int bufsz = 100 * 1024;
	uint8_t *buf = (uint8_t *) malloc (bufsz);

	if (!buf)
	{
		printf("no buffer yet!\n");
		return ;
	}

	startup_mjpeg();

	srv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (srv_sock < 0)
    {
        printf("mjpeg_server: create server socket failed due to (%s)\n",
              strerror(errno));
        goto exit;
    }

    bzero(&addr, sock_len);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = INADDR_ANY;

    /* ignore "socket already in use" errors */
    on = 1;
    lwip_setsockopt(srv_sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    lwip_setsockopt(srv_sock, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));

    if (bind(srv_sock, (struct sockaddr *)&addr, sock_len) != 0)
    {
        printf("mjpeg_server: bind() failed due to (%s)\n",
              strerror(errno));
        goto exit;
    }

    if (listen(srv_sock , RT_LWIP_TCP_PCB_NUM) != 0)
    {
        printf("mjpeg_server: listen() failed due to (%s)\n",
              strerror(errno));
        goto exit;
    }

    g_mjpeg_stop = 0;
    while (!g_mjpeg_stop)
    {
    	struct sockaddr_in client_addr;
		int client = accept(srv_sock, (struct sockaddr *)&client_addr, &sock_len);
		if (client < 0)
			continue;

		printf("mjpeg_server: client connected\n");
		if (send_first_response(client) < 0)
		{
			client = -1;
			continue;
		}

		while (1)
		{
			int len = bufsz;
			/* capture a jpeg frame */
			if (FHAdv_Video_CaptureJpeg(0, (FH_ADDR)buf, &len) == FH_SUCCESS)
			{
				/* send out this frame */
				if (mjpeg_send_stream(client, (void*)buf, len) < 0)
				{
					printf("client disconnected!\n");
					break;
				}
			}
		}
	}

exit:
	if (srv_sock >= 0) lwip_close(srv_sock);
	if (buf) free(buf);
}

int mjpeg(int argc, char** argv)
{
	if (argc != 2)
	{
		printf("%s start|stop\n", argv[0]);
		return 0;
	}

	if (strcmp(argv[1], "start") == 0)
	{
		rt_thread_t tid;

		tid = rt_thread_create("mjpg", mjpeg_server_thread, NULL, 2048, 250, 20);
		if (tid) rt_thread_startup(tid);
	}
	else
	{
		g_mjpeg_stop = 1;
	}

	return 0;
}
MSH_CMD_EXPORT(mjpeg, mjpeg server);

extern int capture_jpeg(unsigned char *buf, int bufsz);
int capture(int argc, char** argv)
{
	uint8_t *buf = RT_NULL;
	int bufsz;

	if (argc != 2)
	{
		rt_kprintf("%s filename.jpg\n", argv[0]);
		return 0;
	}

	bufsz = 100 * 1024;
	buf = (uint8_t *) malloc (bufsz);
	if (buf)
	{
		bufsz = capture_jpeg(buf, bufsz);
		if (bufsz > 0)
		{
			int fd;

			fd = open(argv[1], O_RDWR | O_TRUNC, 0);
			if (fd >= 0)
			{
				write(fd, buf, bufsz);
				close(fd);
			}
		}
		else
		{
			rt_kprintf("capture failed!\n");
		}

		free(buf);
	}
	else
	{
		rt_kprintf("no buffer yet!!!\n");
	}

	return 0;
}
MSH_CMD_EXPORT(capture, capture image to jpeg file);

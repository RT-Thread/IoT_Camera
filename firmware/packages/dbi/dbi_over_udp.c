/*
 * dbi_over_udp.c
 *
 *  Created on: 2016.5.6
 *      Author: gaoyb
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lwip/sockets.h"
#include "types/type_def.h"
// #include <fcntl.h>
#include "di/debug_interface.h"
#include "dbi_over_udp.h"

struct dbi_over_udp
{
    int udp_port;
    int udp_connection;
    char debug_ip[16];

    struct Debug_Interface *di;
};

static int udp_send(void *obj, unsigned char *buf, int size)
{
    struct dbi_over_udp *udp = (struct dbi_over_udp *)obj;

    return sendto(udp->udp_connection, buf, size, 0,
                  (struct sockaddr *)&udp->debug_ip, sizeof(udp->debug_ip));
}

static int udp_recv(void *obj, unsigned char *buf, int size)
{
    socklen_t len;
    struct dbi_over_udp *udp = (struct dbi_over_udp *)obj;

    len = sizeof(udp->debug_ip);

    return recvfrom(udp->udp_connection, buf, size, 0,
                    (struct sockaddr *)&udp->debug_ip, &len);
}

struct dbi_over_udp *udp_dbi_create(int port)
{
    struct dbi_over_udp *udp;
    udp = malloc(sizeof(struct dbi_over_udp));
    memset(udp, 0, sizeof(struct dbi_over_udp));
    strcpy(udp->debug_ip, "0.0.0.0");
    udp->udp_port = port;

    int ret; //, fd;
//    int on = 1;
    struct sockaddr_in local_addr;

    ret = socket(AF_INET, SOCK_DGRAM, 0);
    if (ret < 0)
    {
        perror("socket open failed");
        return NULL;
    }

    udp->udp_connection = ret;

    lwip_fcntl(udp->udp_connection, F_SETFL, O_NONBLOCK);

    local_addr.sin_family      = AF_INET;
    local_addr.sin_port        = htons(port);
    local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    ret = bind(udp->udp_connection, (struct sockaddr *)&local_addr,
               sizeof(local_addr));
    if (ret < 0)
    {
        perror("bind error");
        return NULL;
    }

    struct DI_config di_cfg;

    di_cfg.obj  = udp;
    di_cfg.send = udp_send;
    di_cfg.recv = udp_recv;

    udp->di = DI_create(&di_cfg);

    return udp;
}

int udp_dbi_destroy(struct dbi_over_udp *udp)
{
    lwip_close(udp->udp_connection);
    DI_destroy(udp->di);

    free(udp);
    return 0;
}

int *udp_dbi_thread(struct dbi_udp_config *conf)
{
    int ret;
    int *exit                = conf->cancel;
    struct dbi_over_udp *udp = udp_dbi_create(conf->port);

    while (!*exit)
    {
        ret = DI_handle(udp->di);
        if (ret == -1) continue;
    }

    udp_dbi_destroy(udp);
    return 0;
}

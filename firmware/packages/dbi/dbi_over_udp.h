#ifndef _DBI_OVER_UDP_H
#define _DBI_OVER_UDP_H

struct dbi_udp_config
{
    int port;
    int *cancel;
};

/* dbi_over_udp main thread */
int *udp_dbi_thread(struct dbi_udp_config *conf);

#endif

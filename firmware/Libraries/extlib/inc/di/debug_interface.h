#ifndef DEBUG_INTERFACE_H_
#define DEBUG_INTERFACE_H_

typedef int (*dbi_send)(void *obj, unsigned char *buf, int size);
typedef int (*dbi_recv)(void *obj, unsigned char *buf, int size);

typedef struct DI_config
{
    void *obj;
    dbi_send send;
    dbi_recv recv;

} DI_config;

struct Debug_Interface;

int DI_handle(struct Debug_Interface *di);

int DI_destroy(struct Debug_Interface *di);

/* object create */
struct Debug_Interface *DI_create(struct DI_config *cfg);

#endif

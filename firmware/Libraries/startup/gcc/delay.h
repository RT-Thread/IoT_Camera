#include <rtthread.h>

extern void __bad_udelay(void);
extern void __delay(int loops);
extern void __udelay(int loops);
extern void __const_udelay(rt_uint32_t);

#define MAX_UDELAY_MS 2

#define udelay(n)                                                          \
    (__builtin_constant_p(n)                                               \
         ? ((n) > (MAX_UDELAY_MS * 1000)                                   \
                ? __bad_udelay()                                           \
                : __const_udelay((n) *                                     \
                                 ((2199023U * RT_TICK_PER_SECOND) >> 11))) \
         : __udelay(n))

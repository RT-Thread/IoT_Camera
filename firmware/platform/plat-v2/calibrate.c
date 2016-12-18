#include "fh_def.h"
#include <rtthread.h>
#include <gcc/delay.h>

#if defined(FH_CALIBRATE_DEBUG) && defined(RT_DEBUG)
#define FH_CALIBRATE_DEBUG(fmt, args...)    \
    do                                      \
    {                                       \
        rt_kprintf("FH_CALIBRATE_DEBUG: "); \
        rt_kprintf(fmt, ##args);            \
    } while (0)
#else
#define FH_CALIBRATE_DEBUG(fmt, args...) \
    do                                   \
    {                                    \
    } while (0)
#endif

unsigned long lpj_fine;
unsigned long preset_lpj;
unsigned long loops_per_jiffy = (1 << 12);
static unsigned long calibrate_delay_converge(void);

/*
 * This is the number of bits of precision for the loops_per_jiffy.  Each
 * time we refine our estimate after the first takes 1.5/HZ seconds, so try
 * to start with a good estimate.
 * For the boot cpu we can skip the delay calibration and assign it a value
 * calculated based on the timer frequency.
 * For the rest of the CPUs we cannot assume that the timer frequency is same as
 * the cpu frequency, hence do the calibration for those.
 */
#define LPS_PREC 8

static unsigned long calibrate_delay_converge(void)
{
    /* First stage - slowly accelerate to find initial bounds */
    unsigned long lpj, lpj_base, ticks, loopadd, loopadd_base, chop_limit;
    int trials = 0, band = 0, trial_in_band = 0;

    lpj = (1 << 12);

    /* wait for "start of" clock tick */
    ticks = rt_tick_get();
    while (ticks == rt_tick_get())
        ; /* nothing */
    /* Go .. */
    ticks = rt_tick_get();
    do
    {
        if (++trial_in_band == (1 << band))
        {
            ++band;
            trial_in_band = 0;
        }
        __delay(lpj * band);
        trials += band;
    } while (ticks == rt_tick_get());
    /*
     * We overshot, so retreat to a clear underestimate. Then estimate
     * the largest likely undershoot. This defines our chop bounds.
     */
    trials -= band;
    loopadd_base = lpj * band;
    lpj_base     = lpj * trials;

    FH_CALIBRATE_DEBUG("trials: %d, loopadd_base: %lu, lpj_base: %lu\n", trials,
                       loopadd_base, lpj_base);

recalibrate:
    lpj     = lpj_base;
    loopadd = loopadd_base;

    /*
     * Do a binary approximation to get lpj set to
     * equal one clock (up to LPS_PREC bits)
     */
    chop_limit = lpj >> LPS_PREC;
    while (loopadd > chop_limit)
    {
        lpj += loopadd;
        ticks = rt_tick_get();
        while (ticks == rt_tick_get())
            ; /* nothing */
        ticks = rt_tick_get();
        __delay(lpj);
        if (rt_tick_get() != ticks) /* longer than 1 tick */
            lpj -= loopadd;
        loopadd >>= 1;
    }
    /*
     * If we incremented every single time possible, presume we've
     * massively underestimated initially, and retry with a higher
     * start, and larger range. (Only seen on x86_64, due to SMIs)
     */
    if (lpj + loopadd * 2 == lpj_base + loopadd_base * 2)
    {
        lpj_base = lpj;
        loopadd_base <<= 2;
        goto recalibrate;
    }

    return lpj;
}

void calibrate_delay(void)
{
    unsigned long lpj;
    static rt_bool_t printed;

    if (preset_lpj)
    {
        lpj = preset_lpj;
        if (!printed)
            FH_CALIBRATE_DEBUG(
                "Calibrating delay loop (skipped) "
                "preset value.. ");
    }
    else if ((!printed) && lpj_fine)
    {
        lpj = lpj_fine;
        FH_CALIBRATE_DEBUG(
            "Calibrating delay loop (skipped), "
            "value calculated using timer frequency.. ");
    }
    else
    {
        if (!printed) FH_CALIBRATE_DEBUG("Calibrating delay loop... ");
        lpj = calibrate_delay_converge();
    }

    if (!printed)
        FH_CALIBRATE_DEBUG("%lu.%02lu BogoMIPS (lpj=%lu)\n",
                           lpj / (500000 / RT_TICK_PER_SECOND),
                           (lpj / (5000 / RT_TICK_PER_SECOND)) % 100, lpj);

    loops_per_jiffy = lpj;
    printed         = RT_TRUE;
}

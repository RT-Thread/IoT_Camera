#include "fh_driverlib.h"

void WDT_Enable(struct fh_wdt_obj *wdt_obj, int enable)
{
    SET_REG(wdt_obj->base + WDOG_CONTROL_REG_OFFSET, enable);
}

inline int WDT_IsEnable(struct fh_wdt_obj *wdt_obj)
{
    return GET_REG(wdt_obj->base + WDOG_CONTROL_REG_OFFSET) &
        WDOG_CONTROL_REG_WDT_EN_MASK;
}

void WDT_SetTopValue(struct fh_wdt_obj *wdt_obj, int top)
{
    SET_REG(wdt_obj->base + WDOG_TIMEOUT_RANGE_REG_OFFSET, top);
}

void WDT_SetCtrl(struct fh_wdt_obj *wdt_obj, UINT32 reg)
{
    SET_REG(wdt_obj->base + WDOG_CONTROL_REG_OFFSET, reg);
}

void WDT_Kick(struct fh_wdt_obj *wdt_obj)
{
    SET_REG(wdt_obj->base + WDOG_COUNTER_RESTART_REG_OFFSET, WDOG_COUNTER_RESTART_KICK_VALUE);
}

UINT32 WDT_GetCurrCount(struct fh_wdt_obj *wdt_obj)
{
    return GET_REG(wdt_obj->base + WDOG_CURRENT_COUNT_REG_OFFSET);
}

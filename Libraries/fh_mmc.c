#include "fh_driverlib.h"

// *1: card off
// *0: card on
inline rt_uint32_t MMC_GetCardStatus(struct fh_mmc_obj *mmc_obj)
{
    rt_uint32_t card_status = GET_REG(mmc_obj->base + OFFSET_SDC_CDETECT);

    return card_status & 0x1;
}

inline void MMC_StartDma(struct fh_mmc_obj *mmc_obj)
{
    rt_uint32_t reg;

    SET_REG(mmc_obj->base + OFFSET_SDC_DBADDR, (rt_uint32_t)mmc_obj->descriptors);
    reg = GET_REG(mmc_obj->base + OFFSET_SDC_BMOD);
    reg |= 1 << 7;
    SET_REG(mmc_obj->base + OFFSET_SDC_BMOD, reg);
}

inline void MMC_StopDma(struct fh_mmc_obj *mmc_obj)
{
    rt_uint32_t reg;

    reg = GET_REG(mmc_obj->base + OFFSET_SDC_BMOD);
    reg &= ~(1 << 7);
    SET_REG(mmc_obj->base + OFFSET_SDC_BMOD, reg);
}

void MMC_InitDescriptors(struct fh_mmc_obj *mmc_obj, rt_uint32_t *buf, rt_uint32_t size)
{
    MMC_DMA_Descriptors *desc;
    rt_uint32_t len = 0;
    int i, desc_cnt = 0;

    desc = mmc_obj->descriptors;

    while(size > 0)
    {
        desc[desc_cnt].desc0.bit.own = 1;
        desc[desc_cnt].desc0.bit.sencond_address_chained = 1;
        desc[desc_cnt].desc1.bit.buffer1_size = MIN(MMC_DMA_DESC_BUFF_SIZE, size);
        desc[desc_cnt].desc2.bit.buffer_addr0 = (rt_uint32_t)buf + len;
        desc[desc_cnt].desc3.bit.buffer_addr1 = (rt_uint32_t)mmc_obj->descriptors + (desc_cnt + 1) * sizeof(MMC_DMA_Descriptors);

        size -= desc[desc_cnt].desc1.bit.buffer1_size;
        len += desc[desc_cnt].desc1.bit.buffer1_size;
        desc_cnt++;
    }

    desc[0].desc0.bit.first_descriptor = 1;
    desc[desc_cnt-1].desc0.bit.last_descriptor = 1;
    desc[desc_cnt-1].desc3.bit.buffer_addr1 = 0;
}


inline rt_uint32_t MMC_GetWaterlevel(struct fh_mmc_obj *mmc_obj)
{
    return (GET_REG(mmc_obj->base + OFFSET_SDC_STATUS) >> 17) & 0x1fff;
}

inline rt_uint32_t MMC_GetStatus(struct fh_mmc_obj *mmc_obj)
{
    return GET_REG(mmc_obj->base + OFFSET_SDC_STATUS);
}

inline rt_uint32_t MMC_GetRawInterrupt(struct fh_mmc_obj *mmc_obj)
{
    return GET_REG(mmc_obj->base + OFFSET_SDC_RINTSTS);
}

inline rt_uint32_t MMC_GetUnmaskedInterrupt(struct fh_mmc_obj *mmc_obj)
{
    return GET_REG(mmc_obj->base + OFFSET_SDC_MINTSTS);
}

inline rt_uint32_t MMC_ClearRawInterrupt(struct fh_mmc_obj *mmc_obj, rt_uint32_t interrupts)
{
    return SET_REG(mmc_obj->base + OFFSET_SDC_RINTSTS, interrupts);
}

inline rt_uint32_t MMC_GetInterruptMask(struct fh_mmc_obj *mmc_obj)
{
    return GET_REG(mmc_obj->base + OFFSET_SDC_INTMASK);
}

inline rt_uint32_t MMC_SetInterruptMask(struct fh_mmc_obj *mmc_obj, rt_uint32_t mask)
{
    return SET_REG(mmc_obj->base + OFFSET_SDC_INTMASK, mask);
}

inline void MMC_SetByteCount(struct fh_mmc_obj *mmc_obj, rt_uint32_t bytes)
{
    SET_REG(mmc_obj->base + OFFSET_SDC_BYTCNT, bytes);
}

inline void MMC_SetBlockSize(struct fh_mmc_obj *mmc_obj, rt_uint32_t size)
{
    SET_REG(mmc_obj->base + OFFSET_SDC_BLKSIZ, size);
}

inline rt_uint32_t MMC_GetResponse(struct fh_mmc_obj *mmc_obj, int resp_num)
{
    return GET_REG(mmc_obj->base + OFFSET_SDC_RESP0 + resp_num * 4);
}

inline rt_uint32_t MMC_IsFifoEmpty(struct fh_mmc_obj *mmc_obj)
{
    return (GET_REG(mmc_obj->base + OFFSET_SDC_STATUS) >> 2) & 0x1;
}

inline rt_uint32_t MMC_IsDataStateBusy(struct fh_mmc_obj *mmc_obj)
{
    return (GET_REG(mmc_obj->base + OFFSET_SDC_STATUS) >> 10) & 0x1;
}

int MMC_WriteData(struct fh_mmc_obj *mmc_obj, rt_uint32_t *buf, rt_uint32_t size)
{
    int filled = 0, fifo_available, i, retries;

    for (i=0; i<size/4; i++)
    {
        retries = 0;
        do
        {
            fifo_available = MMC_FIFO_DEPTH - MMC_GetWaterlevel(mmc_obj);
            if(retries++ > 10000)
            {
                rt_kprintf("ERROR: %s, get water level timeout\n", __func__);
                return -RT_ETIMEOUT;
            }
        }
        while(!fifo_available);
        SET_REG(mmc_obj->base + OFFSET_SDC_FIFO, *buf++);
    }

    retries = 0;
    while(MMC_IsDataStateBusy(mmc_obj))
    {
        if(retries++ > 10000)
        {
            rt_kprintf("ERROR: %s, timeout, data line keep being busy\n", __func__);
            return -RT_ETIMEOUT;
        }
    }

    return 0;
}
int MMC_ReadData(struct fh_mmc_obj *mmc_obj, rt_uint32_t *buf, rt_uint32_t size)
{
    int fifo_available, i, retries;

    for (i=0; i<size/4; i++)
    {
        retries = 0;
        do
        {
            fifo_available = MMC_GetWaterlevel(mmc_obj);
            if(retries++ > 10000)
            {
                rt_kprintf("ERROR: %s, get water level timeout\n", __func__);
                return -RT_ETIMEOUT;
            }
        }
        while(!fifo_available);

        *buf++ = GET_REG(mmc_obj->base + OFFSET_SDC_FIFO);
    }

    retries = 0;
    while(MMC_IsDataStateBusy(mmc_obj))
    {
        if(retries++ > 10000)
        {
            rt_kprintf("ERROR: %s, timeout, data line keep being busy\n", __func__);
            return -RT_ETIMEOUT;
        }
    }

    return 0;
}


int MMC_UpdateClockRegister(struct fh_mmc_obj *mmc_obj, int div)
{
    rt_uint32_t tick, timeout;

    tick = rt_tick_get();
    timeout = tick + RT_TICK_PER_SECOND / 10; //100ms in total

    /* disable clock */
    SET_REG(mmc_obj->base + OFFSET_SDC_CLKENA, 0);
    SET_REG(mmc_obj->base + OFFSET_SDC_CLKSRC, 0);

    /* inform CIU */
    SET_REG(mmc_obj->base + OFFSET_SDC_CMD, 1<<31 | 1<<21);
    while(GET_REG(mmc_obj->base + OFFSET_SDC_CMD) & 0x80000000)
    {
        tick = rt_tick_get();
        if(tick > timeout)
        {
            rt_kprintf("ERROR: %s, update clock timeout\n", __func__);
            return -RT_ETIMEOUT;
        }
    }

    /* set clock to desired speed */
    SET_REG(mmc_obj->base + OFFSET_SDC_CLKDIV, div);

    /* inform CIU */
    SET_REG(mmc_obj->base + OFFSET_SDC_CMD, 1<<31 | 1<<21);

    while(GET_REG(mmc_obj->base + OFFSET_SDC_CMD) & 0x80000000)
    {
        tick = rt_tick_get();
        if(tick > timeout)
        {
            rt_kprintf("ERROR: %s, update clock timeout\n", __func__);
            return -RT_ETIMEOUT;
        }
    }

    /* enable clock */
    SET_REG(mmc_obj->base + OFFSET_SDC_CLKENA, 1);

    /* inform CIU */
    SET_REG(mmc_obj->base + OFFSET_SDC_CMD, 1<<31 | 1<<21);

    while(GET_REG(mmc_obj->base + OFFSET_SDC_CMD) & 0x80000000)
    {
        tick = rt_tick_get();
        if(tick > timeout)
        {
            rt_kprintf("ERROR: %s, update clock timeout\n", __func__);
            return -RT_ETIMEOUT;
        }
    }

    return 0;
}

int MMC_SetCardWidth(struct fh_mmc_obj *mmc_obj, int width)
{
    switch(width)
    {
    case MMC_CARD_WIDTH_1BIT:
        SET_REG(mmc_obj->base + OFFSET_SDC_CTYPE, 0);
        break;
    case MMC_CARD_WIDTH_4BIT:
        SET_REG(mmc_obj->base + OFFSET_SDC_CTYPE, 1);
        break;
    default:
        rt_kprintf("ERROR: %s, card width %d is not supported\n", __func__, width);
        return -RT_ERROR;
        break;
    }
    return 0;
}

int MMC_SendCommand(struct fh_mmc_obj *mmc_obj, rt_uint32_t cmd, rt_uint32_t arg, rt_uint32_t flags)
{
    rt_uint32_t reg, tick, timeout;

    tick = rt_tick_get();
    timeout = tick + RT_TICK_PER_SECOND; //1s

    SET_REG(mmc_obj->base + OFFSET_SDC_CMDARG, arg);
    flags |= 1<<31 | 1<<29 | cmd;

    SET_REG(mmc_obj->base + OFFSET_SDC_CMD, flags);

    while(GET_REG(mmc_obj->base + OFFSET_SDC_CMD) & MMC_CMD_START_CMD)
    {
        tick = rt_tick_get();
        if(tick > timeout)
        {
            rt_kprintf("ERROR: %s, send cmd timeout\n", __func__);
            return -RT_ETIMEOUT;
        }
    }

    //fixme: check HLE_INT_STATUS
    return 0;
}

int MMC_ResetFifo(struct fh_mmc_obj *mmc_obj)
{
    rt_uint32_t reg, tick, timeout;

    tick = rt_tick_get();
    timeout = tick + RT_TICK_PER_SECOND / 10; //100ms

    reg = GET_REG(mmc_obj->base + OFFSET_SDC_CTRL);
    reg |= 1 << 1;
    SET_REG(mmc_obj->base + OFFSET_SDC_CTRL, reg);

    //wait until fifo reset finish
    while(GET_REG(mmc_obj->base + OFFSET_SDC_CTRL) & MMC_CTRL_FIFO_RESET)
    {
        tick = rt_tick_get();
        if(tick > timeout)
        {
            rt_kprintf("ERROR: %s, FIFO reset timeout\n", __func__);
            return -RT_ETIMEOUT;
        }
    }

    return 0;
}

int MMC_Reset(struct fh_mmc_obj *mmc_obj)
{
    rt_uint32_t reg, tick, timeout;

    tick = rt_tick_get();
    timeout = tick + RT_TICK_PER_SECOND / 10; //100ms

    reg = GET_REG(mmc_obj->base + OFFSET_SDC_BMOD);
    reg |= MMC_BMOD_RESET;
    SET_REG(mmc_obj->base + OFFSET_SDC_BMOD, reg);

    while(GET_REG(mmc_obj->base + OFFSET_SDC_BMOD) & MMC_BMOD_RESET)
    {
        tick = rt_tick_get();
        if(tick > timeout)
        {
            rt_kprintf("ERROR: %s, BMOD Software reset timeout\n", __func__);
            return -RT_ETIMEOUT;
        }
    }

    reg = GET_REG(mmc_obj->base + OFFSET_SDC_CTRL);
    reg |= MMC_CTRL_CONTROLLER_RESET | MMC_CTRL_FIFO_RESET | MMC_CTRL_DMA_RESET;
    SET_REG(mmc_obj->base + OFFSET_SDC_CTRL, reg);

    tick = rt_tick_get();
    timeout = tick + RT_TICK_PER_SECOND / 10; //100ms
    while(GET_REG(mmc_obj->base + OFFSET_SDC_CTRL) & (MMC_CTRL_CONTROLLER_RESET | MMC_CTRL_FIFO_RESET | MMC_CTRL_DMA_RESET))
    {
        tick = rt_tick_get();
        if(tick > timeout)
        {
            rt_kprintf("ERROR: %s, CTRL dma|fifo|ctrl reset timeout\n", __func__);
            return -RT_ETIMEOUT;
        }
    }
    return 0;
}


void MMC_Init(struct fh_mmc_obj *mmc_obj)
{
    rt_uint32_t reg;

    if(mmc_obj->mmc_reset)
        mmc_obj->mmc_reset(mmc_obj);

    MMC_Reset(mmc_obj);

    //fixed burst
    reg = GET_REG(mmc_obj->base + OFFSET_SDC_BMOD);
    reg |= 1 << 1;
    SET_REG(mmc_obj->base + OFFSET_SDC_BMOD, reg);

    //fixme: power on ? ctrl by gpio ?

    MMC_ClearRawInterrupt(mmc_obj, MMC_INT_STATUS_ALL);
    MMC_SetInterruptMask(mmc_obj, 0x0);

    //fixme: use_internal_dma
    reg = GET_REG(mmc_obj->base + OFFSET_SDC_CTRL);
    reg |= MMC_CTRL_INT_ENABLE;
#ifdef MMC_USE_DMA
    reg |= MMC_CTRL_USE_DMA;
#endif
    SET_REG(mmc_obj->base + OFFSET_SDC_CTRL, reg);

    //set timeout param
    SET_REG(mmc_obj->base + OFFSET_SDC_TMOUT, 0xffffffff);

    //set fifo
    reg = GET_REG(mmc_obj->base + OFFSET_SDC_FIFOTH);
    reg = (reg >> 16) & 0x7ff;
    reg = ((0x2 << 28) | ((reg/2) << 16) | ((reg/2 + 1) << 0));
    SET_REG(mmc_obj->base + OFFSET_SDC_FIFOTH, reg);
}

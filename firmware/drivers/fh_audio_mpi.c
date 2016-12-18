/*
 * File      : fh_audio_mpi.c
 * This file is part of FH8620 BSP for RT-Thread distribution.
 *
 * Copyright (c) 2016 Shanghai Fullhan Microelectronics Co., Ltd.
 * All rights reserved
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *  Visit http://www.fullhan.com to get contact with Fullhan.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2015/12/22     xuww         first version
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <rthw.h>
#include <stdint.h>
#include <time.h>

#include "type_def.h"
#include "acw.h"
#include "fh_audio_mpi.h"
#include "dma_mem.h"

typedef struct
{
    struct rt_dma_device *dam_device;
    rt_device_t audio_fd;
    struct fh_audio_cfg_arg user_cfg;
    FH_AC_IO_TYPE_E ai_type;
    FH_AC_IO_TYPE_E ao_type;
    char *swap_buf;

} AUDIO_DATA_INFO;
static AUDIO_DATA_INFO g_audio_info = {.audio_fd = 0, .swap_buf = 0};

FH_SINT32 FH_AC_Init()
{
    g_audio_info.audio_fd = (rt_device_t)rt_device_find("audio");

    if (g_audio_info.audio_fd < 0)
    {
        PRINT_ACW_MPI_DBG("AUDIO INIT ERROR :AUDIO DEVICE CANNOT FIND\n");
        return -EINVAL;
    }
    g_audio_info.audio_fd->open(g_audio_info.audio_fd, 0);
    g_audio_info.dam_device =
        (struct rt_dma_device *)rt_device_find("fh81_dma");
    if (g_audio_info.dam_device == RT_NULL)
    {
        PRINT_ACW_MPI_DBG("can't find dma dev\n");

        return -EINVAL;
    }
    return RT_EOK;
}
FH_SINT32 FH_AC_DeInit() { return RT_EOK; }
FH_SINT32 FH_AC_Set_Config(struct fh_audio_cfg_arg *pstConfig)
{
    RT_ASSERT(pstConfig != RT_NULL);
    int ret;
    struct fh_audio_cfg_arg hw_cfg;

    if (g_audio_info.swap_buf == RT_NULL ||
        g_audio_info.user_cfg.period_size != pstConfig->period_size)
    {
        if (g_audio_info.swap_buf)
        {
            g_audio_info.swap_buf = RT_NULL;
        }

        g_audio_info.swap_buf =
            (char *)fh_dma_mem_malloc(pstConfig->period_size * 4);

        if (g_audio_info.swap_buf == RT_NULL)
        {
            PRINT_ACW_MPI_DBG("failed to malloc internal swap buffer\n");
            return -RT_ENOMEM;
        }
    }

    if (pstConfig->volume < 0 || pstConfig->volume > 100)
        hw_cfg.volume = g_audio_info.user_cfg.volume = 85;

    hw_cfg.volume = g_audio_info.user_cfg.volume = pstConfig->volume;
    hw_cfg.io_type = g_audio_info.user_cfg.io_type = pstConfig->io_type;
    hw_cfg.rate = g_audio_info.user_cfg.rate = pstConfig->rate;
    g_audio_info.user_cfg.frame_bit          = pstConfig->frame_bit;
    hw_cfg.channels = g_audio_info.user_cfg.channels = pstConfig->channels;
    hw_cfg.period_size = g_audio_info.user_cfg.period_size =
        pstConfig->period_size;
    hw_cfg.buffer_size = g_audio_info.user_cfg.buffer_size =
        g_audio_info.user_cfg.period_size << 3;
    if (pstConfig->io_type == (enum io_select)FH_AC_MIC_IN ||
        pstConfig->io_type == (enum io_select)FH_AC_LINE_IN)
    {
        g_audio_info.ai_type = pstConfig->io_type;

        ret = g_audio_info.audio_fd->control(g_audio_info.audio_fd,
                                             AC_INIT_CAPTURE_MEM, &hw_cfg);

        if (ret < 0)
        {
            PRINT_ACW_MPI_DBG("ioctl AC_INIT_CAPTURE_MEM failed\n");
            return -RT_ERROR;
        }
    }
    else if (pstConfig->io_type == (enum io_select)FH_AC_SPK_OUT ||
             pstConfig->io_type == (enum io_select)FH_AC_LINE_OUT)
    {
        g_audio_info.ao_type = pstConfig->io_type;

        ret = g_audio_info.audio_fd->control(g_audio_info.audio_fd,
                                             AC_INIT_PLAYBACK_MEM, &hw_cfg);

        if (ret < 0)
        {
            PRINT_ACW_MPI_DBG("ioctl AC_INIT_PLAYBACK_MEM failed \n");
            return RT_ERROR;
        }
    }
    else
    {
        PRINT_ACW_MPI_DBG(" invalid io_type %d\n", pstConfig->io_type);
        return RT_ERROR;
    }

    return RT_EOK;
}

FH_SINT32 FH_AC_AI_Enable()
{
    int ret;

    ret = g_audio_info.audio_fd->control(g_audio_info.audio_fd, AC_AI_EN,
                                         RT_NULL);
    if (ret < 0)
    {
        PRINT_ACW_MPI_DBG(" AC_AI_EN failed \n");
        return -EINVAL;
    }

    return RT_EOK;
}
FH_SINT32 FH_AC_AI_Disable()
{
    int ret;

    ret = g_audio_info.audio_fd->control(g_audio_info.audio_fd, AC_AI_DISABLE,
                                         RT_NULL);
    if (ret < 0)
    {
        PRINT_ACW_MPI_DBG(" AC_AI_DISABLE failed \n");
        return -EINVAL;
    }
    return RT_EOK;
}
FH_SINT32 FH_AC_AO_Enable()
{
    int ret;

    ret = g_audio_info.audio_fd->control(g_audio_info.audio_fd, AC_AO_EN,
                                         RT_NULL);
    if (ret < 0)
    {
        PRINT_ACW_MPI_DBG(" AC_AO_EN failed \n");
        return -EINVAL;
    }

    return RT_EOK;
}
FH_SINT32 FH_AC_AO_Disable()
{
    int ret;

    ret = g_audio_info.audio_fd->control(g_audio_info.audio_fd, AC_AO_DISABLE,
                                         RT_NULL);
    if (ret < 0)
    {
        PRINT_ACW_MPI_DBG(" AC_AO_DISABLE failed \n");
        return -EINVAL;
    }

    return RT_EOK;
}
FH_SINT32 FH_AC_AI_SetVol(FH_SINT32 volume)
{
    int ret;
    int vol = volume;

    ret =
        g_audio_info.audio_fd->control(g_audio_info.audio_fd, AC_SET_VOL, &vol);
    if (ret < 0)
    {
        PRINT_ACW_MPI_DBG(" AC_SET_VOL failed \n");
        return -EINVAL;
    }

    return RT_EOK;
}

FH_SINT32 FH_AC_AO_SetVol(FH_SINT32 volume)
{
    int ret;
    int vol = volume;

    ret = g_audio_info.audio_fd->control(g_audio_info.audio_fd, AC_SET_VOL_SPK,
                                         &vol);
    if (ret < 0)
    {
        PRINT_ACW_MPI_DBG(" AC_SET_OUT_VOL failed \n");
        return -EINVAL;
    }

    return RT_EOK;
}

static int filter_redundant_data(char *in, char *out, int len, int bit_num)
{
    char *p   = in;
    char *q   = out;
    int count = len / 4;
    int i;

    if (bit_num == 8)
    {
        for (i = 0; i < count; i++) *(q + i) = *(p + 4 * i + 2);
    }
    if (bit_num == 16)
    {
        for (i = 0; i < count; i++)
        {
            *(q + 2 * i)     = *(p + 4 * i + 1);
            *(q + 2 * i + 1) = *(p + 4 * i + 2);
        }
    }

    return (count * (bit_num / 8));
}

static int extend_data_to_four_byte(char *in, char *out, int len, int bit_num)
{
    char *p   = in;
    char *q   = out;
    int count = len / (bit_num / 8);
    int i;

    if (bit_num == 8)
    {
        for (i = 0; i < count; i++)
        {
            *(q + 4 * i)     = 0;
            *(q + 4 * i + 1) = 0;
            *(q + 4 * i + 2) = *(p + i);
            *(q + 4 * i + 3) = 0;
        }
    }
    if (bit_num == 16)
    {
        for (i = 0; i < count; i++)
        {
            *(q + 4 * i)     = 0;
            *(q + 4 * i + 1) = *(p + 2 * i);
            *(q + 4 * i + 2) = *(p + 2 * i + 1);
            *(q + 4 * i + 3) = 0;
        }
    }

    return (count * 4);
}

FH_SINT32 FH_AC_AI_GetFrame(FH_AC_FRAME_S *pstFrame)
{
    int frame_bytes, valid_len, len_read = 0;
    char *p;
    int ret;
    struct timespec timeout;

    RT_ASSERT(pstFrame);
    RT_ASSERT(pstFrame->data);

    p             = (char *)pstFrame->data;
    pstFrame->len = 0;
    frame_bytes   = g_audio_info.user_cfg.period_size * 4;

    timeout.tv_sec  = 1;
    timeout.tv_nsec = (1000000000 * g_audio_info.user_cfg.period_size) /
                      g_audio_info.user_cfg.rate;

    ret = g_audio_info.audio_fd->control(g_audio_info.audio_fd, AC_READ_SELECT,
                                         &timeout);
    if (ret < 0)
    {
        PRINT_ACW_MPI_DBG(" FH_AC_AI_GetFrame failed %d\n", ret);
        return ret;
    }

    if (g_audio_info.user_cfg.frame_bit != AC_BW_24)
    {
        len_read = g_audio_info.audio_fd->read(
            g_audio_info.audio_fd, 0, g_audio_info.swap_buf, frame_bytes);

        valid_len = filter_redundant_data(g_audio_info.swap_buf, p, len_read,
                                          g_audio_info.user_cfg.frame_bit);

        p += valid_len;
        pstFrame->len += valid_len;
    }
    else
    {
        len_read = g_audio_info.audio_fd->read(g_audio_info.audio_fd, 0, p,
                                               frame_bytes);

        p += len_read;
        pstFrame->len += len_read;
    }

    return RT_EOK;
}

FH_SINT32 FH_AC_AI_GetFrames(FH_AC_FRAME_S **pstFrame, FH_UINT32 frameCount)
{
    int i;
    FH_SINT32 ret = 0;
    for (i = 0; i < frameCount; i++)
    {
        ret = FH_AC_AI_GetFrame(pstFrame[i]);
        if (ret < 0) break;
    }
    return ret;
}

FH_SINT32 FH_AC_CHANG_DAM(AUDIO_DATA_INFO *audio_info)
{
#if 0  // TBD
	 struct rt_dma_device *rt_dma_dev;
	    struct dma_transfer trans = {0};
	    int clk;
	    rt_uint32_t ret;

	        trans.channel_number = 3;
	        rt_kprintf("test channel no:%d\n",trans.channel_number);


	        trans.dma_number = 0;
	        trans.dst_add = &rx_buff[0];
	        trans.dst_inc_mode = DW_DMA_SLAVE_INC;
	        trans.dst_msize = DW_DMA_SLAVE_MSIZE_32;
	        trans.dst_width = DW_DMA_SLAVE_WIDTH_32BIT;
	        trans.fc_mode = DMA_M2M;
	        trans.src_add = &tx_buff[0];

	        trans.src_inc_mode = DW_DMA_SLAVE_INC;
	        trans.src_msize = DW_DMA_SLAVE_MSIZE_32;
	        //p_tx_trans->src_per
	        trans.src_width = DW_DMA_SLAVE_WIDTH_32BIT;
	        trans.trans_len = 1024;
	        trans.src_gather_en = 1;
	        trans.sgr_i =2;
	        trans.period_len = TEST_PER_NO;
	        trans.complete_callback =mem_cut_complete;
	        trans.complete_para = RT_NULL;
	        rt_dma_dev->ops->control(rt_dma_dev,RT_DEVICE_CTRL_DMA_OPEN,(void *)&trans);
	        rt_dma_dev->ops->control(rt_dma_dev,RT_DEVICE_CTRL_DMA_REQUEST_CHANNEL,(void *)&trans);


	        mmu_clean_invalidated_dcache(trans.src_add,trans.trans_len);

	        rt_kprintf("cyclic prepare\n");
	    //    rt_dma_dev->ops->control(rt_dma_dev,RT_DEVICE_CTRL_DMA_CYCLIC_PREPARE,(void *)&trans);
	        rt_kprintf("cyclic start\n");
	        rt_dma_dev->ops->control(rt_dma_dev,RT_DEVICE_CTRL_DMA_SINGLE_TRANSFER,(void *)&trans);
	        rt_kprintf("%d %x\n",__LINE__,*(int *)0xEE000150);
	        rt_thread_sleep(50);
	        rt_dma_dev->ops->control(rt_dma_dev,RT_DEVICE_CTRL_DMA_RELEASE_CHANNEL,(void *)&trans);

#endif
    return 0;
}
FH_SINT32 FH_AC_AO_SendFrame(FH_AC_FRAME_S *pstFrame)
{
    int frame_bytes;
    char *p;
    int ret;
    struct timespec timeout;

    RT_ASSERT(pstFrame);
    RT_ASSERT(pstFrame->data);

    timeout.tv_sec  = 1;
    timeout.tv_nsec = (1000000000 * g_audio_info.user_cfg.period_size) /
                      g_audio_info.user_cfg.rate;

    if (g_audio_info.user_cfg.frame_bit == AC_BW_24)
    {
        if (pstFrame->len != 4 * g_audio_info.user_cfg.period_size)
        {
            PRINT_ACW_MPI_DBG(" FH_AC_AO_SendFrame failed len %d size %d\n",
                              pstFrame->len, g_audio_info.user_cfg.period_size);
            return -EINVAL;
        }
    }

    frame_bytes = g_audio_info.user_cfg.period_size * 4;
    if (g_audio_info.user_cfg.frame_bit != AC_BW_24)
    {
        extend_data_to_four_byte((char *)pstFrame->data, g_audio_info.swap_buf,
                                 pstFrame->len,
                                 g_audio_info.user_cfg.frame_bit);
        p = g_audio_info.swap_buf;
    }
    else
    {
        p = (char *)pstFrame->data;
    }
    ret = g_audio_info.audio_fd->control(g_audio_info.audio_fd, AC_WRITE_SELECT,
                                         &timeout);
    if (ret < 0)
    {
        PRINT_ACW_MPI_DBG(" FH_AC_AI_SendFrame failed \n");
        return ret;
    }

    g_audio_info.audio_fd->write(g_audio_info.audio_fd, 0, p, frame_bytes);

    return RT_EOK;
}
#define FH_MPI_TEST 0
#if FH_MPI_TEST
struct fh_audio_cfg_arg mpi_cfg;
int fh_mpi_test()
{
    PRINT_ACW_MPI_DBG(" mpi_test \n");
    mpi_cfg.buffer_size = 1024 * 8;
    mpi_cfg.channels    = 0;
    mpi_cfg.frame_bit   = AC_BW_24;
    mpi_cfg.io_type     = mic_in;
    mpi_cfg.period_size = 1024;
    mpi_cfg.rate        = 48000;
    mpi_cfg.volume      = 80;

    FH_AC_FRAME_S get_frame_info;
    FH_AC_FRAME_S send_frame_info;

    get_frame_info.data = rt_malloc(mpi_cfg.period_size * 8 * 4);
    if (get_frame_info.data == RT_NULL) PRINT_ACW_MPI_DBG(" no mem \n");
    send_frame_info.data = get_frame_info.data;
    if (send_frame_info.data == RT_NULL) PRINT_ACW_MPI_DBG(" no mem \n");
    if (mpi_cfg.frame_bit == AC_BW_24)
    {
        get_frame_info.len  = mpi_cfg.period_size * 4;
        send_frame_info.len = mpi_cfg.period_size * 4;
    }
    else if (mpi_cfg.frame_bit == AC_BW_16)
    {
        get_frame_info.len  = mpi_cfg.period_size * 2;
        send_frame_info.len = mpi_cfg.period_size * 2;
    }
    else
    {
        get_frame_info.len  = mpi_cfg.period_size;
        send_frame_info.len = mpi_cfg.period_size;
    }

    FH_AC_Init();
    FH_AC_Set_Config(&mpi_cfg);
    mpi_cfg.io_type = line_out;
    FH_AC_Set_Config(&mpi_cfg);
    FH_AC_AO_Enable();
    FH_AC_AI_Enable();
    PRINT_ACW_MPI_DBG(" mpi 0x0 = %x\n", readl(0xf0a00000));
    PRINT_ACW_MPI_DBG(" mpi 0x4 = %x\n", readl(0xf0a00004));
    PRINT_ACW_MPI_DBG(" mpi 0x8 = %x\n", readl(0xf0a00008));
    PRINT_ACW_MPI_DBG(" mpi 0xc = %x\n", readl(0xf0a0000c));
    PRINT_ACW_MPI_DBG(" mpi 0x10 = %x\n", readl(0xf0a00010));
    PRINT_ACW_MPI_DBG(" mpi 0x14 = %x\n", readl(0xf0a00014));
    PRINT_ACW_MPI_DBG(" mpi 0x18 = %x\n", readl(0xf0a00018));
    PRINT_ACW_MPI_DBG(" mpi 0x1c = %x\n", readl(0xf0a0001c));
    PRINT_ACW_MPI_DBG(" mpi 0x20 = %x\n", readl(0xf0a00020));

    while (1)
    {
        FH_AC_AI_GetFrame(&get_frame_info);
        FH_AC_AO_SendFrame(&send_frame_info);
    }
}
#ifdef RT_USING_FINSH
#include <finsh.h>

FINSH_FUNCTION_EXPORT(fh_mpi_test, fh_mpi_test);
#endif
#endif

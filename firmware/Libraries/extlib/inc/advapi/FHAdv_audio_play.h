/**
 * @file     FHAdv_audio_play.h
 * @brief    FHAdv audio play module interface
 * @version  V1.0.0
 * @date     11-May-2016
 * @author   Software Team
 *
 * @note
 * Copyright (C) 2016 Shanghai Fullhan Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * @par
 * Fullhan is supplying this software which provides customers with programming
 * information regarding the products. Fullhan has no responsibility or
 * liability for the use of the software. Fullhan not guarantee the correctness
 * of this software. Fullhan reserves the right to make changes in the software
 * without notification.
 *
 */

#ifndef _FHADV_AUDIO_PLAY_H
#define _FHADV_AUDIO_PLAY_H

#include "FH_typedef.h"

/** @addtogroup Audio_Play Audio Play
 *  @{
 */

/**
 *  @brief 		Audio Sample rate for play engine
 */
typedef enum {
    FHADV_AUPLY_8000  = 8000,  /*!< 8kHz */
    FHADV_AUPLY_16000 = 16000, /*!< 16kHz */
    FHADV_AUPLY_44100 = 44100, /*!< 44.1kHz */
    FHADV_AUPLY_48000 = 48000, /*!< 48kHz */
} FHADV_AUPLY_SAMPLERATE;

/**
*  @brief 		Sample bit depth for play engine
*/
typedef enum {
    FHADV_AUPLY_8BIT  = 8,  /*!< 8 bits per sample */
    FHADV_AUPLY_16BIT = 16, /*!< 16 bits per sample */
} FHADV_AUPLY_BITDEPTH;

/**
*  @brief 		Output source
*/
typedef enum {
    FHADV_AUPLY_SPK_OUT  = 2,
    FHADV_AUPLY_LINE_OUT = 3,
} FHADV_AUCAP_OUTPUT_INTERFACE;

/**
 *  @brief 		Audio Play Configure
 */
typedef struct
{
    FHADV_AUPLY_SAMPLERATE sample_rate;     /*!< sample rate */
    FHADV_AUPLY_BITDEPTH bit_depth;         /*!< sample bit depth */
    FH_SINT32 volume;                       /*!< play volume 0~100, (-1) using default value */
    FHADV_AUCAP_OUTPUT_INTERFACE output_if; /*!< output interface type */
} FHADV_AUPLY_CONFIG_t;

/**
 *  @brief      Audio Play engine Open
 *
 *              The configuration can only be set once. Changing configuration such as
 *              sample_rate dynamically  is not supported.
 *              This function will fail when it is called at the second time.
 *
 * @param[in]	pConfig	Pointer of Audio Play Configure
 * @return 		-errno
 */
FH_SINT32 FHAdv_Auply_Open(FHADV_AUPLY_CONFIG_t *pConfig);

/**
 *  @brief 		Audio Play engine Close
 *
 * @return 		-errno
 */
FH_SINT32 FHAdv_Auply_Close(void);

/**
 *  @brief 		Play audio data
 *
 * 				This function works on BLOCKING mode, i.e. the function will not
 *				returned until all the data have been moved to playback buffer.
 *				And the function will be called again, to transfer next slice of
 *				data.\n
 *				The PCM data format is needed. For 16bit data, sample is stored
 *				in little-endian.\n
 *				The time interval of TWO calls of this function should NOT exceed
 *				500ms for continuous playing.
 *
 * @param[in]	data	audio buffer pointer
 * @param[in]	len		audio buffer length
 * @return 		-errno
 */
FH_SINT32 FHAdv_Auply_Play(FH_ADDR data, FH_SINT32 len);

/**
 *  @brief 		Set audio play volume
 *
 * @param[in]	vol 	Volume for audio play, range [0,100]
 * @return 		-errno
 */
FH_SINT32 FHAdv_Auply_SetVolume(FH_SINT32 vol);

/*@} end of group Audio_Play */

#endif

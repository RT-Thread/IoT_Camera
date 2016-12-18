/**
 * @file     FHAdv_audio_cap.h
 * @brief    FHAdv audio capture module interface
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

#ifndef _FHADV_AUDIO_CAP_H
#define _FHADV_AUDIO_CAP_H

#include "FH_typedef.h"

/** @addtogroup Audio_Cap Audio Capture
 *  @{
 */

#define FHADV_AUCAP_NOERR FH_SUCCESS
#define FHADV_AUCAP_ENODEV FH_FAILURE

/**
*  @brief 		Current frame information returned by audio capture engine
*/
typedef struct
{
    FH_UINT64 ts; /*!< capture time stamp */
} FHADV_AUCAP_FRM_INFO_t;

/**
 *  @brief 		Get one audio frame callback function
 *
 * 				This function will be called after one audio frame captured.
 *
 * @param[in]	info    Pointer of frame information structure
 * @param[in]	data    Pointer of stream buffer
 * @param[in]	len     The length of stream buffer in bytes
 * @return 		NULL
 */
typedef void (*FHAdv_Aucap_GetFrame_cb)(const FHADV_AUCAP_FRM_INFO_t *info, const FH_ADDR data, const FH_SINT32 len);

/**
*  @brief 		Audio Sample rate
*/
typedef enum {
    FHADV_AUCAP_8000  = 8000,  /*!< 8kHz */
    FHADV_AUCAP_16000 = 16000, /*!< 16kHz */
    FHADV_AUCAP_44100 = 44100, /*!< 44.1kHz */
    FHADV_AUCAP_48000 = 48000, /*!< 48kHz */
} FHADV_AUCAP_SAMPLERATE;

/**
*  @brief 		Sample bit depth
*/
typedef enum {
    FHADV_AUCAP_8BIT  = 8,  /*!< 8 bits per sample */
    FHADV_AUCAP_16BIT = 16, /*!< 16 bits per sample */
} FHADV_AUCAP_BITDEPTH;

/**
*  @brief 		Input source
*/
typedef enum {
    FHADV_AUCAP_MIC_IN  = 0,
    FHADV_AUCAP_LINE_IN = 1,
} FHADV_AUCAP_INPUT_INTERFACE;

/**
*  @brief 		Audio Capture Configure
*/
typedef struct
{
    FHADV_AUCAP_SAMPLERATE sample_rate;   /*!< sample rate */
    FHADV_AUCAP_BITDEPTH bit_depth;       /*!< sample bit depth */
    FH_SINT32 volume;                     /*!< capture volume 0~100, (-1) using default value */
    FHAdv_Aucap_GetFrame_cb cb;           /*!< get audio frame callback */
    FHADV_AUCAP_INPUT_INTERFACE input_if; /*!< input source type */
} FHADV_AUCAP_CONFIG_t;

/**
 *  @brief      Audio Capture engine Open
 *
 *              The configuration can only be set once. Changing configuration such as
 *              sample_rate dynamically  is not supported.
 *              This function will fail when it is called at the second time.
 *
 * @param[in]   pConfig Pointer of Audio Capture Configure
 * @return      -errno
 */
FH_SINT32 FHAdv_Aucap_Open(FHADV_AUCAP_CONFIG_t *pConfig);

/**
 *  @brief 		Audio Capture engine Close
 *
 * @return 		-errno
 */
FH_SINT32 FHAdv_Aucap_Close(void);

/**
 *  @brief 		Activate audio capture engine, callback will be triggered periodically
 *
 * @return 		-errno
 */
FH_SINT32 FHAdv_Aucap_Start(void);

/**
 *  @brief 		Deactivate audio capture engine
 *
 * @return 		-errno
 */
FH_SINT32 FHAdv_Aucap_Stop(void);

/**
 *  @brief 		Set audio capture volume
 *
 * @param[in]	vol 	Volume for audio capture, range [0,100]
 * @return 		-errno
 */
FH_SINT32 FHAdv_Aucap_SetVolume(FH_SINT32 vol);

/**
 *  @brief 		Turn on/off AEC ( Acoustic Echo Canceller )
 *
 * @param[in]	enable 	1: turn on, 0: turn off
 * @return 		-errno
 * @retval		FHADV_AUCAP_NOERR	Setting success
 * @retval		-FHADV_AUCAP_ENODEV	AEC not support
 */
FH_SINT32 FHAdv_Aucap_AecEnable(FH_SINT32 enable);

/*@} end of group Audio_Cap */

#endif

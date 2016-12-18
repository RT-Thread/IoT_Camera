/**
 * @file     FHAdv_video_cap.h
 * @brief    FHAdv video capture module interface
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

#ifndef _FHADV_VIDEO_CAP_H
#define _FHADV_VIDEO_CAP_H

#include "FH_typedef.h"

/** @addtogroup Video_Cap Video Capture
 *  @{
 */

/**
*  @brief 		Current frame information returned by video engine
*/
typedef struct
{
    FH_UINT64 ts;        /*!< capture time stamp */
    FH_SINT32 key_frame; /*!< indicate whether or not current frame is I frame */
} FHADV_VIDEO_FRM_INFO_t;

/**
 *  @brief 		Get one frame callback function
 *
 * 				This function will be called after one frame encoding finished.\n
 *				The data should be a NALU started with "00 00 00 01" delimiter.\n
 *				A valid keyframe should contain SPS and PPS.
 *
 * @param[in]	info    Pointer of frame information structure
 * @param[in]	data    Pointer of stream buffer
 * @param[in]	len     The length of stream buffer in bytes
 * @return 		Null
 */
typedef void (*FHAdv_Video_GetFrame_cb)(const FHADV_VIDEO_FRM_INFO_t *info, const FH_ADDR data, const FH_SINT32 len);

/**
*  @brief 		Video resolution
*/
typedef enum {
    FHADV_VIDEO_RES_INVALID = 0, /*!< invalid resolution */
    FHADV_VIDEO_RES_720P,        /*!< resolution 1280x720 */
    FHADV_VIDEO_RES_480P,        /*!< resolution 720x480 */
    FHADV_VIDEO_RES_360P,        /*!< resolution 640x360 */
} FHADV_VIDEO_RESOLUTION;

/**
*  @brief 		Rate control mode
*/
typedef enum {
    FHADV_RC_CBR = 0, /*!< constant bitrate control */
    FHADV_RC_VBR,     /*!< variable bitrate control */
} FHADV_RC_MODE;

/**
*  @brief 		Video Channel information
*/
typedef struct
{
    FH_UINT32 channelId;        /*!< channel ID */
    FHADV_VIDEO_RESOLUTION res; /*!< resolution */
    FH_UINT32 fps;              /*!< frame per second (fps) */
    FH_UINT32 bitrate;          /*!< bitrate in kbps */
    FH_UINT32 gop;              /*!< group of picture (gop) */
    FHADV_RC_MODE rcmode;       /*!< rate control mode */
    FHAdv_Video_GetFrame_cb cb; /*!< get one frame callback */
} FHADV_VIDEO_CHANNEL_t;

/**
 *  @brief 		Video engine Open
 *
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_Open(void);

/**
 *  @brief 		Video engine Close
 *
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_Close(void);

/**
 *  @brief 		Add one video encoding channel into video engine
 *
 * 				FH8620 only TWO channels are support, the main channel and the second channel.\n
 *				The resolution of the second channel should NOT exceed FHADV_VIDEO_RES_480P.
 *
 * @param[in]	ch		Pointer of Video Channel structure
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_AddChannel(FHADV_VIDEO_CHANNEL_t *ch);

/**
 *  @brief 		Remove video encoding channel from video engine
 *
 * 				You can remove and re-add one channel without enlarging the resolution. i.e.\n
 *				"Add channel 1 with FHADV_VIDEO_RES_360P. remove channel 1, add channel 1 with
 *				FHADV_VIDEO_RES_480P" is NOT permitted.\n
 *				"Add channel 1 with FHADV_VIDEO_RES_480P. remove channel 1, add channel 1 with
 *				FHADV_VIDEO_RES_360P" is allowed.
 *
 * @param[in]	channel		Channel ID. (-1) means remove all existed channels from video engine
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_RemoveChannel(FH_SINT32 channel);

/**
 *  @brief 		Activate certain channel to start video encoding
 *
 * @param[in]	channel		Channel ID. (-1) means start all existed channels in video engine
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_Start(FH_SINT32 channel);

/**
 *  @brief 		Stop certain channel from video encoding
 *
 * @param[in]	channel		Channel ID. (-1) means stop all existed channels in video engine
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_Stop(FH_SINT32 channel);

/**
 *  @brief 		Set rate control parameter
 *
 * @param[in]	channel		Channel ID.
 * @param[in]	bitrate		bitrate in kbps
 * @param[in]	rcmode		rate control mode
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_SetRateControl(FH_SINT32 channel, FH_SINT32 bitrate, FHADV_RC_MODE rcmode);

/**
 *  @brief 		Set flip and mirror.
 *
 * 				All the channel will be affected simultaneously.
 *
 * @param[in]	flip		image flip (up side down)
 * @param[in]	mirror		image mirror (left side right)
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_SetFlipMirror(FH_BOOL flip, FH_BOOL mirror);

/**
*  @brief 		Rotate mode
*/
typedef enum {
    FHADV_ROTATE_0   = 0,   /*!< Rotate 0 degree */
    FHADV_ROTATE_90  = 90,  /*!< Rotate 90 degree */
    FHADV_ROTATE_180 = 180, /*!< Rotate 180 degree */
    FHADV_ROTATE_270 = 270, /*!< Rotate 270 degree */
} FHADV_ROTATE_MODE;

/**
 *  @brief 		Set Rotate mode. Encoded picture can be rotated by 0/90/180/270 degree
 *
 * @param[in]	channel		Channel ID.
 * @param[in]	rotate		output stream rotate (degree in clockwise)
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_SetRotate(FH_SINT32 channel, FHADV_ROTATE_MODE rotate);

/**
 *  @brief 		Force I Frame. Next frame will be forced to be encoded in I (Key) frame
 *
 * @param[in]	channel		Channel ID.
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_ForceIFrame(FH_SINT32 channel);

/**
 *  @brief 		Capture JPEG
 *
 * 				The resolution is fixed with channel 0.\n
 *				The quality is control by SDK automatically.\n
 *				The maximum JPEG size should never exceed 96KiB.
 *
 * @param[in]	channel		channel ID
 * @param[in]	buf			JPEG restore buffer address
 * @param[out]	bufLen		Return total JPEG buffer in bytes
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_CaptureJpeg(FH_SINT32 channel, FH_ADDR buf, FH_SINT32 *bufLen);

/**
 *  @brief 		Capture YUV
 *
 *				The YUV format is YUV420 Planar.\n
 * 				The resolution (**w** and **h**) of captured YUV has follow constrain:\n
                                 - 1280x720 is allowed if some channel with FHADV_VIDEO_RES_720P is activated
                                 - 720x480 is allowed if some channel with FHADV_VIDEO_RES_480P is activated
                                 - any **w** and **h** meet following condition is allowed
                                        1. w <= 640, H <= 360
                                        2. w%8 = 0, h%8 = 0
 *
 *				ATTENTION: The minimum buffer size allocated should be __w * h * 1.5__
 *
 * @param[in]	w			Picture width
 * @param[in]	h			Picture height
 * @param[in]	buf			YUV restore buffer address
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_CaptureYUV(FH_SINT32 w, FH_SINT32 h, FH_ADDR buf);

/**
*  @brief 		Day night status
*/
typedef enum {
    FHADV_VIDEO_UNKNOWN = -1, /*!< status unknown */
    FHADV_VIDEO_NIGHT   = 0,  /*!< night scene */
    FHADV_VIDEO_DAY,          /*!< day scene */
} FHADV_VIDEO_DAYNIGHT;

/**
 *  @brief 		Get day night status
 *
 * @return 		FHADV_VIDEO_DAYNIGHT
 */
FHADV_VIDEO_DAYNIGHT FHAdv_Video_GetDayNight(void);

/**
*  @brief 		Infra-red mode.
*/
typedef enum {
    FHADV_IR_UNSUPPORT = -1, /*!< do not support Day/Night control */
    FHADV_IR_AUTO      = 0,  /*!< color/mono switched automatically */
    FHADV_IR_COLOR     = 1,  /*!< always color image,  */
    FHADV_IR_MONO      = 2,  /*!< always mono image */
} FHADV_IR_MODE;

/**
 *  @brief 		Set IR mode. The default mode is FHADV_IR_AUTO
 *
 * 				The image should be switched mono when turn on IR LED.
 *
 * @param[in]	mode		IR control mode
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_SetIRMode(FHADV_IR_MODE mode);

/**
 *  @brief 		Get current IR mode
 *
 * @return 		FHADV_IR_MODE
 */
FHADV_IR_MODE FHAdv_Video_GetIRMode(void);

/**
*  @brief 		OSD information
*
* 				Thera are two separate layers of OSD support in this interface.\n
*				Small picture: a picture or logo with maximum size 128x128, and
*				ARGB1555 format.\n
*				Time: yyyy-mm-dd hh:mm:ss
*/
typedef struct
{
    FH_BOOL pic_enable; /*!< enable picture display */
    FH_SINT32 pic_x;    /*!< picture coordinate x  (up left corner is 0,0) */
    FH_SINT32 pic_y;    /*!< picture coordinate y  */
    FH_SINT32 pic_w;    /*!< picture width  */
    FH_SINT32 pic_h;    /*!< picture height  */
    FH_ADDR buf;        /*!< picture buffer pointer (ARGB1555) */

    FH_BOOL time_enable; /*!< enable time display */
    FH_SINT32 time_x;    /*!< time coordinate x  (up left corner is 0,0) */
    FH_SINT32 time_y;    /*!< time coordinate y  */
} FHADV_VIDEO_OSD_t;

/**
 *  @brief 		Set OSD information
 *
 * @param[in]	channel		Channel ID.
 * @param[in]	pOsdInfo	Pointer of OSD information structure
 * @return 		-errno
 */
FH_SINT32 FHAdv_Video_SetOSD(FH_SINT32 channel, FHADV_VIDEO_OSD_t *pOsdInfo);

/*@} end of group Video_Cap */

#endif

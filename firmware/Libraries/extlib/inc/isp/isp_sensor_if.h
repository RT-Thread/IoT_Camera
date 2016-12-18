#ifndef _ISP_SENSOR_IF_H_
#define _ISP_SENSOR_IF_H_

#include "types/type_def.h"
#include "isp_common.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

struct Sensor_Resolution_S
{
    FH_SINT32 active_height;
    FH_SINT32 active_width;
};

struct Sensor_Frame_S
{
    FH_SINT32 frame_height;
    FH_SINT32 frame_width;
};

typedef struct _Sensor_Input_Cfg_S
{
    struct Sensor_Resolution_S resolution;
    struct Sensor_Frame_S frame;

} SENSOR_INPUT_CFG_S;

/**
 * @brief		ISP sensor 接口，每个sensor模块应该使用该接口以便与ISP对接，通过安装sensor，
 *				将其加入ISP Core中。
 */
struct isp_sensor_if  // module interface
{
    FH_SINT8* name; /*!< sensor名称 */

    /**
     * @brief		设置sensor增益
     * @param[in]	gain sensor的增益，6为小数（0x40 表示 x1.0）
     * @return 		无
     */
    FH_SINT32 (*set_gain)(FH_UINT32 gain);

    /**
     * @brief		获取配置幅面信息
     * @param[in]	无
     * @return 		幅面信息
     */
    FH_SINT32 (*get_vi_attr)(ISP_VI_ATTR_S* vi_attr);

    /**
     * @brief		获取sensor增益
     * @param[in]	无
     * @return 		当前sensor的增益，6为小数（0x40 表示 x1.0）
     */
    FH_SINT32 (*get_gain)(FH_UINT32* u32Gain);

    /**
     * @brief		设置sensor曝光时间
     * @param[in]	intt sensor的曝光时间，按行为单位，4为小数（0x10 表示 1.0 * row_time）
     * @return 		无
     */
    FH_SINT32 (*set_intt)(FH_UINT32 intt);

    /**
     * @brief		设置sensor框架高度
     * @param[in]	multiple sensor框架高度的倍数，用于通过降低帧率来增加曝光时间
     * @return 		无
     */
    FH_SINT32 (*set_frame_height)(FH_UINT32 multiple);

    /**
     * @brief		获取sensor曝光时间
     * @param[in]	无
     * @return 		当前sensor的曝光时间，按行为单位，4为小数（0x10 表示 1.0 * row_time）
     */
    FH_SINT32 (*get_intt)(FH_UINT32* intt);

    /**
     * @brief		设置sensor图像的水平翻转和垂直翻转
     * @param[in]	sensor_en_stat 最低位为flip(vertical)翻转 次低位为mirror(horizontal)翻转
     * @return 		无
     */
    FH_SINT32 (*set_flipmirror)(FH_UINT32 sensor_en_stat);

    /**
     * @brief		获取sensor图像的水平翻转和垂直翻转
     * @param[out]	sensor_en_stat 最低位为flip(vertical)翻转 次低位为mirror(horizontal)翻转
     * @return 		无
     */
    FH_SINT32 (*get_flipmirror)(FH_UINT32* sensor_en_stat);

    /**
     * @brief		设置光圈大小
     * @param[in]	无
     * @return 		设置光圈大小
     */
    FH_SINT32 (*set_iris)(FH_UINT32 iris);

    /**
     * @brief		sensor初始化
     * @param[in]	无
     * @return 		无
     */
    FH_SINT32 (*init)(FH_VOID);

    /**
     * @brief		sensor复位
     * @param[in]	无
     * @return 		无
     */
    FH_VOID (*reset)(FH_VOID);

    /**
     * @brief		sensor退出
     * @param[in]	无
     * @return 		无
     */
    FH_SINT32 (*deinit)(FH_VOID);

    FH_SINT32 (*set_sns_fmt)(int format);

    FH_SINT32 (*set_sns_reg)(FH_UINT16 addr, FH_UINT16 data);
};

struct isp_sensor_if* Sensor_Create(void);
FH_VOID Sensor_Destroy(struct isp_sensor_if* s_if);

FH_UINT32* GetSensorAwbGain(int idx);
FH_UINT32* GetSensorLtmCurve(int idx);
FH_UINT32* GetDefaultParam();
FH_UINT32* GetMirrorFlipBayerFormat();
FH_UINT32* GetMirrorFlipRgbaBayerFormat();
FH_UINT32* GetContrast();
FH_UINT32* GetSaturation();
FH_UINT32* GetSharpness();

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _ISP_SENSOR_H_ */

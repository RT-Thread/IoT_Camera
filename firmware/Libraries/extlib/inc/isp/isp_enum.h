#ifndef ISP_ENUM_H_
#define ISP_ENUM_H_

#define ISP_WAIT_VD 0
#define ISP_WAIT_FRAME_EDGE 1
#define ISP_PROC_RUN 2
#define ISP_GET_VERSION 3
#define ISP_GET_AE_STAT 4
#define ISP_GET_AWB_STAT 5

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum IPB_LTM_CURVE_TYPE
{
    LTM_CURVE_LINEAR,
    LTM_CURVE_60DB,
    LTM_CURVE_80DB,
    LTM_CURVE_100DB,
    LTM_CURVE_120DB,
    LTM_CURVE_USER01,
    LTM_CURVE_USER02,
    LTM_CURVE_USER03,
    LTM_MANUAL_CURVE = 0xF,
};

enum MISC_DATA_COMP
{
    MISC_COMP_NONE,
    MISC_COMP_LINEAR_12BIT,
    MISC_COMP_LINEAR_10BIT,
    MISC_COMP_NORMAL,
    MISC_COMP_USER,
};
enum FORMAT
{

    FORMAT_720P25 = 0,
    FORMAT_720P30 = 1,
    FORMAT_960P25 = 2,
    FORMAT_960P30 = 3,

    FORMAT_VGAP25 = 10,
    FORMAT_VGAP30 = 11,
    //
    SENSOR_FORMAT_MAX,
};

enum CIS_CLK
{
    CIS_CLK_18M,
    CIS_CLK_24M,
    CIS_CLK_27M,
    CIS_CLK_36M,
    CIS_CLK_43M2,  // 43.2M
    CIS_CLK_54M,
    CIS_CLK_72M,
    CIS_CLK_108M,
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* ISP_ENUM_IF_H_ */

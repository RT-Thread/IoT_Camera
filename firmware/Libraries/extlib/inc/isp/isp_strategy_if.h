#ifndef _ISP_STRATEGY_IF_H_
#define _ISP_STRATEGY_IF_H_

#include "types/type_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

/**
 * @brief		ISP策略模块基类，每个策略模块应该继承该类，并初始化。通过安装策略，
 *				将其加入ISP Core中。
 */
struct isp_strategy  // module interface
{
    FH_SINT8* name;          /*!< 策略名称 */
    FH_VOID (*run)(FH_VOID); /*!< 策略运行函数指针 */
};

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* _ISP_STRATEGY_H_ */

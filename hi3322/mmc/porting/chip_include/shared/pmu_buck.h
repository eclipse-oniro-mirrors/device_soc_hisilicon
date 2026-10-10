/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Description:  PMU DRIVER HEADER FILE.
 */

#ifndef PMU_BUCK_H
#define PMU_BUCK_H

#include <stdbool.h>
#include <stdint.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @if Eng
 * @brief  PMU BUCK0 V1P9 voltage range.
 * @else
 * @brief  PMU BUCK0 V1P9电压范围
 * @endif
 */
typedef enum {
    PMU_1P9_VSET_1V7   = 16,    /** @if Eng  PMU V1P9 1.7v.
                                     @else  PMU V1P9 1.7v. @endif */
    PMU_1P9_VSET_1V8   = 20,    /** @if Eng  PMU V1P9 1.8v.
                                     @else  PMU V1P9 1.8v. @endif */
    PMU_1P9_VSET_1V9   = 24,    /** @if Eng  PMU V1P9 1.9v.
                                     @else  PMU V1P9 1.9v. @endif */
    PMU_1P9_VSET_2V0   = 27,    /** @if Eng  PMU V1P9 2.0v.
                                     @else  PMU V1P9 2.0v. @endif */
    PMU_1P9_VSET_2V1   = 30,    /** @if Eng  PMU V1P9 2.1v.
                                     @else  PMU V1P9 2.1v. @endif */
    PMU_1P9_VSET_2V125 = 31,    /** @if Eng  PMU V1P9 2.1v.
                                     @else  PMU V1P9 2.1v. @endif */
} pmu_1p9_vset_t;

/**
 * @if Eng
 * @brief  PMU BUCK V0P9 voltage range.
 * @else
 * @brief  PMU BUCK V0P9电压范围
 * @endif
 */
typedef enum {
    PMU_0P9_VSET_0V6   = 0,    /** @if Eng  PMU V0P9 0.6v.
                                     @else  PMU V0P9 0.6v. @endif */
    PMU_0P9_VSET_0V7   = 4,    /** @if Eng  PMU V0P9 0.7v.
                                     @else  PMU V0P9 0.7v. @endif */
    PMU_0P9_VSET_0V7_25  = 5,    /** @if Eng  PMU V0P9 0.725.
                                     @else  PMU V0P9 0.725v. @endif */
    PMU_0P9_VSET_0V75  = 6,    /** @if Eng  PMU V0P9 0.75.
                                     @else  PMU V0P9 0.75v. @endif */
    PMU_0P9_VSET_0V8   = 8,    /** @if Eng  PMU V0P9 0.8v.
                                     @else  PMU V0P9 0.8v. @endif */
    PMU_0P9_VSET_0V8_25 = 9,    /** @if Eng  PMU V0P9 0.825v.
                                     @else  PMU V0P9 0.825v. @endif */
    PMU_0P9_VSET_0V9   = 12,    /** @if Eng  PMU V0P9 0.9v.
                                     @else  PMU V0P9 0.9v. @endif */
    PMU_0P9_VSET_1V0   = 16,    /** @if Eng  PMU V0P9 1.0v.
                                     @else  PMU V0P9 1.0v. @endif */
    PMU_0P9_VSET_1V1   = 20,    /** @if Eng  PMU V0P9 1.1v.
                                     @else  PMU V0P9 1.1v. @endif */
    PMU_0P9_VSET_1V2   = 24,    /** @if Eng  PMU V0P9 1.2v.
                                     @else  PMU V0P9 1.2v. @endif */
    PMU_0P9_VSET_1V3   = 28,    /** @if Eng  PMU V0P9 1.3v.
                                     @else  PMU V0P9 1.3v. @endif */
    PMU_0P9_VSET_1V4   = 31,    /** @if Eng  PMU V0P9 1.4v.
                                     @else  PMU V0P9 1.4v. @endif */
} pmu_0p9_vset_t;

/**
 * @if Eng
 * @brief  PMU BUCK2 V1P0 voltage range.
 * @else
 * @brief  PMU BUCK2 V1P0电压范围
 * @endif
 */
typedef enum {
    PMU_1P0_VSET_0V6   = 0,    /** @if Eng  PMU V1P0 0.6v.
                                     @else  PMU V1P0 0.6v. @endif */
    PMU_1P0_VSET_0V7   = 4,    /** @if Eng  PMU V1P0 0.7v.
                                     @else  PMU V1P0 0.7v. @endif */
    PMU_1P0_VSET_0V8   = 8,    /** @if Eng  PMU V1P0 0.8v.
                                     @else  PMU V1P0 0.8v. @endif */
    PMU_1P0_VSET_0V9   = 12,    /** @if Eng  PMU V1P0 0.9v.
                                     @else  PMU V1P0 0.9v. @endif */
    PMU_1P0_VSET_1V0   = 16,    /** @if Eng  PMU V1P0 0.91v.
                                     @else  PMU V1P0 0.9v1. @endif */
    PMU_1P0_VSET_1V1   = 20,    /** @if Eng  PMU V1P0 1.1v.
                                     @else  PMU V1P0 1.1v. @endif */
    PMU_1P0_VSET_1V2   = 24,    /** @if Eng  PMU V1P0 1.2v.
                                     @else  PMU V1P0 1.2v. @endif */
    PMU_1P0_VSET_1V3   = 28,    /** @if Eng  PMU V1P0 1.3v.
                                     @else  PMU V1P0 1.3v. @endif */
    PMU_1P0_VSET_1V4   = 31,    /** @if Eng  PMU V1P0 1.4v.
                                     @else  PMU V1P0 1.4v. @endif */
} pmu_1p0_vset_t;

/**
 * @if Eng
 * @brief  Set buck or ldo state.
 * @param  [in]  bus BUCK LDO power plane.
 * @param  [in]  state BUCK LDO state. For details, see @ref pmu_buck_ldo_state_t
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置BUCK LDO状态
 * @param  [in]  bus BUCK LDO电源平面
 * @param  [in]  state BUCK LDO 状态 参考 @ref pmu_buck_ldo_state_t
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败 参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_pmu_buck_ldo_set_state(uint32_t bus, uint8_t state);

/**
 * @if Eng
 * @brief  Set normal mode buck or ldo voltage.
 * @param  [in]  bus BUCK LDO power plane.
 * @param  [in]  voltage voltage value.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置nomal模式下BUCK LDO电压
 * @param  [in]  bus BUCK LDO电源平面
 * @param  [in]  voltage 电压值
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败 参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_pmu_buck_ldo_set_voltage(uint32_t bus, uint8_t voltage);

/**
 * @if Eng
 * @brief  Get normal mode buck or ldo voltage.
 * @param  [in]  bus BUCK LDO power plane.
 * @param  [out]  voltage voltage value.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置normal模式下BUCK LDO电压
 * @param  [in]  bus BUCK LDO电源平面
 * @param  [out]  voltage 电压值
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败 参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_pmu_buck_ldo_get_voltage(uint32_t bus, uint8_t *voltage);

/**
 * @if Eng
 * @brief  Set eco mode buck or ldo voltage.
 * @param  [in]  bus BUCK LDO power plane.
 * @param  [in]  voltage voltage value.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置eco模式下BUCK LDO电压
 * @param  [in]  bus BUCK LDO电源平面
 * @param  [in]  voltage 电压值
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败 参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_pmu_buck_ldo_set_eco_voltage(uint32_t bus, uint8_t voltage);

/**
 * @if Eng
 * @brief  Get eco mode buck or ldo voltage.
 * @param  [in]  bus BUCK LDO power plane.
 * @param  [out]  voltage voltage value.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  设置eco模式下BUCK LDO电压
 * @param  [in]  bus BUCK LDO电源平面
 * @param  [out]  voltage 电压值
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败 参考 @ref errcode_t
 * @endif
 */
errcode_t uapi_pmu_buck_ldo_get_eco_voltage(uint32_t bus, uint8_t *voltage);

/**
 * @if Eng
 * @brief  Pmu buck optimize config.
 * @else
 * @brief  PMU BUCK优化配置
 * @endif
 */
void uapi_pmu_buck_opt(void);

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved. \n
 *
 * Description: Provides sema port \n
 */
#ifndef SEAM_PORTING_H
#define SEAM_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "securec.h"
#include "errcode.h"
#include "common_def.h"
#include "platform_core.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_sema
 * @ingroup  drivers_port
 * @{
 */

/*
 * 信号量寄存器地址连续，基地址为SEM0的地址，
 * 每个信号量有2个寄存器:STS寄存器和FORCE CLR寄存器
 * 1A21规格为8个信号量
 */
#define SEMA_STS_OFFSET          0
#define SEMA_FRC_CLR_OFFSET      4
/* 每组sema寄存器占用的字节数 */
#define SEMA_SIZE                8

#define SEMA_CTL_BASE_ADDR       0x5700019C

/* 各子系统获取和释放信号量的bit位 */
#if (CORE == APPS)
#define SEMA_CTRL_SUBSYS_MASK    0
#elif (CORE == BT)
#define SEMA_CTRL_SUBSYS_MASK    1
#endif

/* 各子系统信号量状态bit位 */
#if (CORE == APPS)
#define SEMA_STS_SUBSYS_MASK     8
#elif (CORE == BT)
#define SEMA_STS_SUBSYS_MASK     9
#endif

/* 各信号量强制释放操作bit位 */
#define SEMA_FORCE_CLR_MASK      8

/**
 * @brief  Definition of type-sema.
 */
typedef enum {
    SEMA_0    = 0, // for tsensor loadswitch
    SEMA_1    = 1,
    SEMA_2    = 2,
    SEMA_3    = 3,
    SEMA_4    = 4,
    SEMA_6    = 6,
    SEMA_7    = 7,
    SEMA_BUTT
}sema_index_t;


/**
 * @if Eng
 * @brief  Initialize the SEMA of porting.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 */
void sema_porting_init(void);

/**
 * @if Eng
 * @brief  Deinitialize the SEMA of porting.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 */
void sema_porting_deinit(void);

/**
 * @brief  Check whether the index configured for the sem is valid.
 * @param  [in]  sema_index  The index of sems. see @ref sema_index_t
 * @return The value 'true' indicates that the mode is valid and the value 'false' indicates that the mode is invalid.
 */
bool sema_check_index_is_valid(uint8_t sema_index);

/**
 * @brief  Get the status register address of the sems.
 * @param  [in]  sema_index  The index of sems. see @ref sema_index_t
 * @return The address of the status register of sems.
 */
uint32_t sema_get_sem_status_regaddr(uint8_t sema_index);

/**
 * @brief  Get the force clear register address of the sems.
 * @param  [in]  sema_index  The index of sems. see @ref sema_index_t
 * @return The address of the force clear register of sems.
 */
uint32_t sema_get_sem_forceclr_regaddr(uint8_t sema_index);

/**
 * @if Eng
 * @brief  Set the sema_set param.
 * @param  [in] sema_index Index of Signal. see @ref sema_index_t.
 * @param  [in] wait_time_us Wait time len of Set the sub_clr param.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取信号量锁存。
 * @param  [in] sema_index 信号量索引，参考 @ref sema_index_t 。
 * @param  [in] wait_time_us 获取信号量锁存的超时等待时间。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t sema_porting_get(uint8_t sema_index, uint32_t wait_time_us);

/**
 * @if Eng
 * @brief  Set the sub_clr param.
 * @param  [in] sema_index Index of Signal. see @ref sema_index_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置信号量释放锁存标记参数。
 * @param  [in] sema_index 信号量索引，参考 @ref sema_index_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t sema_porting_put(uint8_t sema_index);

/**
 * @if Eng
 * @brief  Set the force_clr param.
 * @param  [in] sema_index Index of Signal. see @ref sema_index_t.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  设置信号量强制清除锁存标记参数。
 * @param  [in] sema_index 信号量索引，参考 @ref sema_index_t 。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t sema_porting_force_clear(uint8_t sema_index);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
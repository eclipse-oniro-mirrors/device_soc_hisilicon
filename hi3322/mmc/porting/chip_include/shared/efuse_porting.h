/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 * Description: Provides efuse port template
 *
 * Create: 2023-03-04
 */
#ifndef EFUSE_PORTING_H
#define EFUSE_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_efuse Efuse
 * @ingroup  drivers_port
 * @{
 */

#define EFUSE_REGION_NUM               2
#define EFUSE_REGION_MAX_BITS          1248 // efuse0
#define EFUSE_REGION_MAX_BYTES         (EFUSE_REGION_MAX_BITS >> 3)  // MAX_BIT / 8
#define EFUSE_MAX_BITS                 2272
#define EFUSE_MAX_BYTES                (EFUSE_MAX_BITS >> 3)  // MAX_BIT / 8
#define EFUSE_MAX_BIT_POS              8U
#define EFUSE_DIE_ID_BASE_BYTE_ADDR    44

typedef enum {
    HAL_EFUSE_REGION_0,
    HAL_EFUSE_REGION_1,
    HAL_EFUSE_REGION_MAX,
} hal_efuse_region_t;

#define EFUSE_IDX_NRW 0x0
#define EFUSE_IDX_RO  0x1
#define EFUSE_IDX_WO  0x2
#define EFUSE_IDX_RW  0x3

#define EFUSE_READ_MAX_BYTE     32
#define BURN_EFUSE_BIN_ADDR     (APP_DTCM_ORIGIN + APP_DTCM_LENGTH - 0x1000)
#define EFUSE_CFG_MAX_LEN       1320
#define EFUSE_CFG_MIN_LEN       48

typedef enum {
    EFUSE_DIE_ID = 0,
    EFUSE_CHIP_ID,
    EFUSE_OEM_HASH_ROOT_PUBLIC_KEY_ID,
    EFUSE_MSID_ID,
    EFUSE_OEM_HASH_ROOT_PUBLIC_KEY_LOCK_ID,
    EFUSE_TEE_HASH_ROOT_PUBLIC_KEY_ID,
    EFUSE_TEE_HASH_ROOT_PUBLIC_KEY_LOCK_ID,
    EFUSE_BOOT_VER_ID,
    EFUSE_TEE_VER_ID,
    EFUSE_CHIP_LIFECYCLE_STS,
    EFUSE_SEC_VERIFY_ENABLE,
    EFUSE_SEC_MRK_OEM_ID,
    EFUSE_SEC_MRK_OEM_LOCK_ID,
    EFUSE_XOTRIM0_VAL_ID,
    EFUSE_IDX_MAX,
} efuse_idx;

/**
 * @if Eng
 * @brief  Base address for the EFUSE boot done.
 * @else
 * @brief  EFUSE的上电完成地址
 * @endif
 */
extern uint32_t g_efuse_boot_done_addr;

/**
 * @if Eng
 * @brief  Base address for the IP.
 * @else
 * @brief  IP的基地址
 * @endif
 */
extern uint32_t g_efuse_base_addr[EFUSE_REGION_NUM];

/**
 * @if Eng
 * @brief  Base read address for the IP.
 * @else
 * @brief  IP的读基地址
 * @endif
 */
extern uint32_t g_efuse_region_read_address[EFUSE_REGION_NUM];

/**
 * @if Eng
 * @brief  Base write address for the IP.
 * @else
 * @brief  IP的写基地址
 * @endif
 */
extern uint32_t g_efuse_region_write_address[EFUSE_REGION_NUM];

/**
 * @if Eng
 * @brief  Register hal funcs objects into hal_efuse module.
 * @else
 * @brief  将hal funcs对象注册到hal_efuse模块中
 * @endif
 */
void efuse_port_register_hal_funcs(void);

/**
 * @if Eng
 * @brief  Unregister hal funcs objects from hal_efuse module.
 * @else
 * @brief  从hal_efuse模块注销hal funcs对象
 * @endif
 */
void efuse_port_unregister_hal_funcs(void);

/**
 * @brief  Get the region of a otp byte address
 * @param  byte_addr the addr of the byte to get register
 * @retval region The region of otp
 * @else
 * @brief  获取otp字节地址的区域
 * @param  byte_addr 要获取寄存器的字节的地址
 * @retval 区域OTP的区域
 */
hal_efuse_region_t hal_efuse_get_region(uint32_t byte_addr);

/**
 * @brief  Get the offset addr of a otp byte address
 * @param  byte_addr the addr of the byte to get register
 * @retval address
 * @else
 * @brief  获取otp字节地址的偏移地址
 * @param  byte_addr 要获取寄存器的字节的地址
 * @retval 偏移地址
 */
uint16_t hal_efuse_get_byte_offset(uint32_t byte_addr);

#ifdef CONFIG_EFUSE_READ_TO_RAM
/**
 * @brief  Get the address of a otp byte address saved in ram
 * @param  byte_addr the address of the byte to get register
 * @retval address
 * @else
 * @brief  获取存储在内存中的otp字节地址的偏移地址
 * @param  byte_addr 要获取寄存器的字节的地址
 * @retval 地址
 */
uint8_t *efuse_porting_get_save_addr(uint32_t byte_addr);
#endif

/**
 * @brief  Get the value from efuse
 * @param  efuse_idx the index of the byte to get in map
 * @param  data the value of the byte to get
 * @param  data_len the length of the byte to get
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  从efuse中读取指定字节
 * @param  efuse_idx 要获取字节在表中的序号
 * @param  data 要获取的字节返回的值
 * @param  data_len 要获取的字节的长度
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 */
uint32_t efuse_read_item(efuse_idx efuse_id, uint8_t *data, uint16_t data_len);

/**
 * @brief  check secure boot if enable
 * @param  image_owner trust chain owner
 * @retval ERRCODE_SUCC   Success.
 * @retval Other        Failure. For details, see @ref errcode_t
 * @else
 * @brief  检查安全启动选项是否打开
 * @param  image_owner 信任链持有者
 * @retval ERRCODE_SUCC 成功
 * @retval Other        失败，参考 @ref errcode_t
 */
errcode_t check_verify_enable(uint32_t image_owner);

/**
 * @if Eng
 * @brief  Reload the efuse data before reads multiple bytes from the eFuse into the provided buffer.
 * @param  [in] buffer The value of the bit read.
 * @param  [in] byte_number The source eFuse bit byte_number of the bit to be read.
 * @param  [in] length The length of the data, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  重新从efuse源头加载数据后，再从eFuse中读取多个字节，进入提供的缓冲区。
 * @param  [in] buffer 保存读取数据的缓冲区。
 * @param  [in] byte_number 要读取的数据的初始源eFuse字节地址。
 * @param  [in] length 数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t efuse_port_read_buffer_by_reload(uint8_t *buffer, uint32_t byte_number, uint16_t length);

/**
 * @if Eng
 * @brief  Get chip version.
 * @param  [in] buffer The value of the bit read.
 * @param  [in] length The length of the data, in bytes.
 * @retval ERRCODE_SUCC Success.
 * @retval Other        Failure. For details, see @ref errcode_t.
 * @else
 * @brief  获取芯片版本号.
 * @param  [in] buffer 保存读取数据的缓冲区。
 * @param  [in] length 数据的长度，以字节为单位。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t uapi_efuse_get_chip_version(uint8_t *buffer, uint16_t length);

/**
 * @if Eng
 * @brief  Set efuse write power.
 * @param  [in] power_on power on or power off.
 * @else
 * @brief  设置Efuse写电源.
 * @param  [in] power_on 打开或关闭电源。
 * @endif
 */
void efuse_port_power_on(bool power_on);

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

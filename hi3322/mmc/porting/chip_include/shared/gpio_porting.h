/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides gpio port template \n
 *
 * History: \n
 * 2022-07-26， Create file. \n
 */
#ifndef GPIO_PORTING_H
#define GPIO_PORTING_H

#include <stdint.h>
#include <stdbool.h>
#include "platform_types.h"
#include "hal_gpio_v150_comm_def.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_port_gpio_v100 GPIO V100
 * @ingroup  drivers_port_gpio
 * @{
 */

#if (CORE == APPS)
#define GPIO_CHANNELS_NUM 5
#else
#define GPIO_CHANNELS_NUM 1
#endif

/**
 * @brief  GPIO info definition of each channel and group. Developer should adapt GPIO info here.
 * @note   GPIO_CHANNEL_X_GROUP_NUM:           GPIO group number of channel X.
 *         GPIO_CHANNEL_X_GROUP_Y_PIN_NUM:     GPIO pin number in channel X group Y.
 *         GPIO_CHANNEL_X_GROUP_Y_CB_START_ID: GPIO callback start ID in channel X group Y. The callback functions
 *                                             of all GPIO pins are registered in the same array. The array length
 *                                             is equal to the number of all GPIO pins. The value starts from 0 and
 *                                             is accumulated based on the number of pins in each group.
 *         GPIO_CHANNEL_X_PIN_NUM:             GPIO number of channel X.
 *
 *         GPIO_PIN_NUM:                       GPIO sum number of all channel.
 */
// channel0: S_EGPIO0 ~ S_EGPIO25, 26 pins.
#if (CORE == APPS)
#define GPIO_CHANNEL_0_GROUP_NUM            1
#define GPIO_CHANNEL_0_GROUP_0_PIN_NUM      28
#define GPIO_CHANNEL_0_GROUP_0_CB_START_ID  0

#define GPIO_CHANNEL_1_GROUP_NUM            1
#define GPIO_CHANNEL_1_GROUP_0_PIN_NUM      27
#define GPIO_CHANNEL_1_GROUP_0_CB_START_ID  (GPIO_CHANNEL_0_GROUP_0_PIN_NUM)

#define GPIO_CHANNEL_2_GROUP_NUM            1
#define GPIO_CHANNEL_2_GROUP_0_PIN_NUM      32
#define GPIO_CHANNEL_2_GROUP_0_CB_START_ID  (GPIO_CHANNEL_1_GROUP_0_CB_START_ID + GPIO_CHANNEL_1_GROUP_0_PIN_NUM)

#define GPIO_CHANNEL_3_GROUP_NUM            1
#define GPIO_CHANNEL_3_GROUP_0_PIN_NUM      3
#define GPIO_CHANNEL_3_GROUP_0_CB_START_ID  (GPIO_CHANNEL_2_GROUP_0_CB_START_ID + GPIO_CHANNEL_2_GROUP_0_PIN_NUM)

#define GPIO_CHANNEL_4_GROUP_NUM            1
#define GPIO_CHANNEL_4_GROUP_0_PIN_NUM      2
#define GPIO_CHANNEL_4_GROUP_0_CB_START_ID  (GPIO_CHANNEL_3_GROUP_0_CB_START_ID + GPIO_CHANNEL_3_GROUP_0_PIN_NUM)

#define GPIO_PIN_NUM                                                                                    \
    (GPIO_CHANNEL_0_GROUP_0_PIN_NUM + GPIO_CHANNEL_1_GROUP_0_PIN_NUM + GPIO_CHANNEL_2_GROUP_0_PIN_NUM + \
        GPIO_CHANNEL_3_GROUP_0_PIN_NUM + GPIO_CHANNEL_4_GROUP_0_PIN_NUM)
#endif // CORE == APPS

#if (CORE == BT)
#define GPIO_CHANNEL_0_GROUP_NUM            1
#define GPIO_CHANNEL_0_GROUP_0_PIN_NUM      20
#define GPIO_CHANNEL_0_GROUP_0_CB_START_ID  0
#define GPIO_PIN_NUM (GPIO_CHANNEL_0_GROUP_0_PIN_NUM)
#endif // CORE == BT

#define SEL_BIT_0        0
#define SEL_BIT_1        1
#define SEL_BIT_2        2
#define SEL_BIT_3        3
#define SEL_BIT_4        4
#define SEL_BIT_5        5
#define SEL_BIT_6        6
#define SEL_BIT_7        7
#define SEL_BIT_8        8
#define SEL_BIT_9        9
#define SEL_BIT_10       10
#define SEL_BIT_11       11
#define SEL_BIT_12       12
#define SEL_BIT_13       13
#define SEL_BIT_14       14
#define SEL_BIT_15       15
#define SEL_BIT_16       16
#define SEL_BIT_17       17
#define SEL_BIT_18       18
#define SEL_BIT_19       19

#define GROUP_MAX_PIN_NUM          32
#define BT_GPIO_BASE_ADDR          0x57013000

#define GPIO_DIRECTION_REG_OFFSET  0x4
#define GPIO_HIGN_LEVEL_REG_OFFSET 0x30
#define GPIO_LOW_LEVEL_REG_OFFSET  0x34

/**
 * @brief  Definition of GPIO Channel index.
 */
typedef enum gpio_channel {
    GPIO_CHANNEL_0,
#if (GPIO_CHANNELS_NUM > 1)
    GPIO_CHANNEL_1,
#endif
#if (GPIO_CHANNELS_NUM > 2)
    GPIO_CHANNEL_2,
#endif
#if (GPIO_CHANNELS_NUM > 3)
    GPIO_CHANNEL_3,
#endif
#if (GPIO_CHANNELS_NUM > 4)
    GPIO_CHANNEL_4,
#endif
    GPIO_CHANNEL_MAX_NUM
} gpio_channel_t;

typedef enum gpios_need_map {
#if (CORE == APPS)
    MEM_HS_GPIO_12 = 12,
    MEM_HS_GPIO_13,
    MEM_HS_GPIO_14,
    MEM_HS_GPIO_15,
    MEM_HS_GPIO_16,
    MEM_HS_GPIO_17,
    MEM_HS_GPIO_18,
    MEM_HS_GPIO_19,
    MEM_HS_GPIO_20,
    MEM_HS_GPIO_21,
    MEM_HS_GPIO_22,
    MEM_HS_GPIO_23,
    MEM_HS_GPIO_24,
    MEM_HS_GPIO_25,
    MEM_HS_GPIO_26,
    MEM_HS_GPIO_27,
#endif
    GPIO_MAP_BUTT
} gpios_need_map_t;

/**
 * @brief  Definition map of pin index to GPIO index.
 */
typedef struct pin_to_gpio {
    uint8_t pin_index;
    uint8_t gpio_index;
} pin_to_gpio_t;

/**
 * @brief  Definition map of pin index to core select bit.
 */
typedef struct pin_to_core_sel {
    uint8_t pin_index;
    uint8_t core_sel;
} pin_to_core_sel_t;

typedef enum core_chn_type {
    BT_CHN,
    SRV_CHN_MAX
} core_chn_type_t;

/**
 * @brief  Definition the gpio opt type.
 */
typedef enum srv_core_gpio_optype {
    OP_SET_DIR,
    OP_SET_LEVEL,
    OP_BUTT
} srv_core_gpio_optype_t;

typedef void (*hal_gpio_callback_t)(pin_t pin, uintptr_t param);

/**
 * @brief  Get GPIO channel info.
 * @param  [in]  channel The channel id of GPIO.
 * @return GPIO group info of target channel. See @ref hal_gpio_channel_info_t
 */
hal_gpio_channel_info_t *gpio_porting_channel_info_get(uint32_t channel);

/**
 * @brief  Get GPIO group context of target channel and group.
 * @param  [in]  channel The channel id of GPIO.
 * @param  [in]  group The group id of GPIO.
 * @return GPIO group context of target channel and group. See @ref hal_gpio_group_context_t
 */
hal_gpio_group_context_t *gpio_porting_group_context_get(uint32_t channel, uint32_t group);

/**
 * @brief  Clean all GPIO context of target channel.
 * @param  [in]  channel The channel id of GPIO.
 * @param  [in]  group_num Group number of GPIO channel.
 */
void gpio_porting_channel_context_clean(uint32_t channel, uint32_t group_num);

/**
 * @brief  Get the base address of a specified GPIO.
 * @param  [in]  channel The channel of GPIO.
 * @return The base address of specified GPIO.
 */
uintptr_t gpio_porting_base_addr_get(gpio_channel_t channel);

/**
 * @brief  Enable ulp gpio interrupt, set ulp gpio clk as 32K.
 * @param  on True enable and set clk as 32K, false disable ulp gpio interrupt.
 */
void gpio_ulp_int_en(bool on);

/**
 * @brief  Get the gpio index of current pin.
 * @param  pin_index pin sequence. See @ref pin_t
 */
uint8_t gpio_index_trans_by_pin(uint8_t pin_index);

/**
 * @brief  Get the pin index of current gpio.
 * @param  gpio_index gpio sequence. See @ref pin_t
 */
uint8_t gpio_porting_index_trans_by_gpio(uint8_t gpio_index);

#if defined(CONFIG_GPIO_SELECT_CORE)
/**
 * @brief   Select the core of current pin.
 * @param  pin pin sequence. See @ref pin_t
 * @param  core current core type. See @ref cores_t
 */
void gpio_select_core(pin_t pin, cores_t core);
#endif

/**
 * @brief   Mcpu set gpios of service cores.
 * @param  pin pin sequence. See @ref pin_t
 * @param  op  service core type. See @ref srv_core_gpio_optype_t
 * @param  val config value, dir or level of gpio.
 */
void gpio_porting_service_core_handle(pin_t pin, srv_core_gpio_optype_t op, uint8_t val);
/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif

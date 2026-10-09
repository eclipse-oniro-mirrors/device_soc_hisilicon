/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:   Chip core irq >= LOCAL_INTERRUPT0 + 0 define.
 *
 * Create:  2021-04-21
 */

#ifndef CHIP_CORE_IRQ_H
#define CHIP_CORE_IRQ_H

#define LOCAL_INTERRUPT0 16
#define DMA_IRQN M_DMA_IRQN
typedef enum core_irq {
/* -------------------  Processor Interrupt Numbers  ------------------------------ */
    BT_INT0_IRQN                         = LOCAL_INTERRUPT0 + 0,
    BT_INT1_IRQN                         = LOCAL_INTERRUPT0 + 1,
    DSP0_INT0_IRQN                       = LOCAL_INTERRUPT0 + 2,
    DSP0_INT1_IRQN                       = LOCAL_INTERRUPT0 + 3,
    MCU_TE_IRQN                          = LOCAL_INTERRUPT0 + 4,
    DSI_TE_AGPIO_IRQN                    = LOCAL_INTERRUPT0 + 5,
    MCPU_PCLR_OK_IRQN                    = LOCAL_INTERRUPT0 + 6,
    SEC_TEE_INT0                         = LOCAL_INTERRUPT0 + 7,
    GPIO_0_IRQN                          = LOCAL_INTERRUPT0 + 8,    // A2MCU_GPIO_INTR0
    GPIO_1_IRQN                          = LOCAL_INTERRUPT0 + 9,    // A2MCU_GPIO_INTR1
    SEC_REE_INT0                         = LOCAL_INTERRUPT0 + 10,
    SEC_REE_INT1                         = LOCAL_INTERRUPT0 + 11,
    ULP_VSET_INT                         = LOCAL_INTERRUPT0 + 12,   // GPIO0_INTR_WIFI
    SEC_TEE_INT1                         = LOCAL_INTERRUPT0 + 13,
    SEC_TEE_INT2                         = LOCAL_INTERRUPT0 + 14,
    UART_0_IRQN                          = LOCAL_INTERRUPT0 + 15,   // UART_H0_INT
    UART_1_IRQN                          = LOCAL_INTERRUPT0 + 16,   // UART_H1_INT uart bus0 is uart1
    MEM2MCU_QSPI0_2CS_IRQN               = LOCAL_INTERRUPT0 + 17,
    MEM2MCU_QSPI1_2CS_IRQN               = LOCAL_INTERRUPT0 + 18,
    QSPI2_CS_IRQN                        = LOCAL_INTERRUPT0 + 19,
    AON_PWM_CFG_INT                      = LOCAL_INTERRUPT0 + 20,
    M_WAKEUP_IRQN                        = LOCAL_INTERRUPT0 + 21,
    M_SLEEP_IRQN                         = LOCAL_INTERRUPT0 + 22,
    UART_2_IRQN                          = LOCAL_INTERRUPT0 + 23,    // UART2
    SEC_TEE_INT3                         = LOCAL_INTERRUPT0 + 24,
    PWM_CFG_IRQN                         = LOCAL_INTERRUPT0 + 25,
    RTC_0_IRQN                           = LOCAL_INTERRUPT0 + 26,
    TIMER_0_IRQN                         = LOCAL_INTERRUPT0 + 27,
    TIMER_1_IRQN                         = LOCAL_INTERRUPT0 + 28,
    TIMER_2_IRQN                         = LOCAL_INTERRUPT0 + 29,
    PWM_ABNOR_IRQN                       = LOCAL_INTERRUPT0 + 30,
    M_SDMA_IRQN                          = LOCAL_INTERRUPT0 + 31,
    M_DMA_IRQN                           = LOCAL_INTERRUPT0 + 32,
    SPI1_MS_IRQN                         = LOCAL_INTERRUPT0 + 33,
    SPI2_MS_IRQN                         = LOCAL_INTERRUPT0 + 34,
    SEC_TEE_INT4                         = LOCAL_INTERRUPT0 + 35,
    I2C_0_IRQN                           = LOCAL_INTERRUPT0 + 36,
    I2C_1_IRQN                           = LOCAL_INTERRUPT0 + 37,
    I2C_2_IRQN                           = LOCAL_INTERRUPT0 + 38,
    I2C_3_IRQN                           = LOCAL_INTERRUPT0 + 39,
    MAD_IRQN                             = LOCAL_INTERRUPT0 + 40,
    MEM_HS_GPIO_INT                      = LOCAL_INTERRUPT0 + 41,
    NUM_INTERRUPT_GMMU                   = LOCAL_INTERRUPT0 + 42,
    SEC_TEE_INT5                         = LOCAL_INTERRUPT0 + 43,
    SEC_TEE_INT6                         = LOCAL_INTERRUPT0 + 44,
    B2MCU_WDT_IRQN                       = LOCAL_INTERRUPT0 + 45,
    AON_PWM_ABNOR_INT                    = LOCAL_INTERRUPT0 + 46,
    SCD_NORM_IRQN                        = LOCAL_INTERRUPT0 + 47,
    SCD_SAFE_IRQN                        = LOCAL_INTERRUPT0 + 48,
    VDH_NORM_IRQN                        = LOCAL_INTERRUPT0 + 49,
    VDH_SAFE_IRQN                        = LOCAL_INTERRUPT0 + 50,
    SEC_TEE_INT7                         = LOCAL_INTERRUPT0 + 51,
    PMU_CMU_ERR_IRQN                     = LOCAL_INTERRUPT0 + 52,
    M_GPIO_IRQN                          = LOCAL_INTERRUPT0 + 53,
    ADC_IRQN                             = LOCAL_INTERRUPT0 + 54,
    ULP2MCU_INT                          = LOCAL_INTERRUPT0 + 55,
    B_SUB_MONITOR_IRQN                   = LOCAL_INTERRUPT0 + 56,
    USB_VBUS_IN_INTR_A2M                 = LOCAL_INTERRUPT0 + 57,
    SEC_REE_INT2                         = LOCAL_INTERRUPT0 + 58,
    SEC_REE_INT3                         = LOCAL_INTERRUPT0 + 59,
    SDIO2MCU_SDIO_H_IRQN                 = LOCAL_INTERRUPT0 + 60,
    SEC_REE_INT4                         = LOCAL_INTERRUPT0 + 61,
    SDIO2MCU_EMMC_IRQN                   = LOCAL_INTERRUPT0 + 62,
    SEC_REE_INT5                         = LOCAL_INTERRUPT0 + 63,
    SEC_REE_INT6                         = LOCAL_INTERRUPT0 + 64,
    VOL_IRQ                              = LOCAL_INTERRUPT0 + 65,
    NUM_INTERRUPT_DPU                    = LOCAL_INTERRUPT0 + 66,
    NUM_INTERRUPT_VAU                    = LOCAL_INTERRUPT0 + 67,
    NUM_INTERRUPT_MIPI                   = LOCAL_INTERRUPT0 + 68,
    VIDEO_VDP1_TE_INT                    = LOCAL_INTERRUPT0 + 69,
    NUM_INTERRUPT_JPEG                   = LOCAL_INTERRUPT0 + 70,
    USB_SYS_IRQN                         = LOCAL_INTERRUPT0 + 71,
    M_TTCAN_INT0                         = LOCAL_INTERRUPT0 + 72,
    M_TTCAN_INT1                         = LOCAL_INTERRUPT0 + 73,
    D2MCU_DSP_WDG0_IRQN                  = LOCAL_INTERRUPT0 + 74,
    SEC_REE_INT7                         = LOCAL_INTERRUPT0 + 75,
    AIC_DFX_AF_INT_MCPU_IRQN             = LOCAL_INTERRUPT0 + 76,
    AIC_NORM_INT_MCPU_IRQN               = LOCAL_INTERRUPT0 + 77,
    AIC_EXCEPT_INT_MCPU_IRQN             = LOCAL_INTERRUPT0 + 78,
    AIC_DEBUG_INT_MCPU_IRQN              = LOCAL_INTERRUPT0 + 79,
    AIC_AICPU_INT_MCPU_IRQN              = LOCAL_INTERRUPT0 + 80,
    COMRAM_MONITOR_IRQN                  = LOCAL_INTERRUPT0 + 81,
    MEM_SUB_MONITOR_IRQN                 = LOCAL_INTERRUPT0 + 82,
    TCM_MONITOR_IRQN                     = LOCAL_INTERRUPT0 + 83,
    EH2H_IRQN                            = LOCAL_INTERRUPT0 + 84,
    A2MCU_CALI_32K_IRQN                  = LOCAL_INTERRUPT0 + 85,
    COM_UART_INT_IRQN                    = LOCAL_INTERRUPT0 + 86,
    TEE_TIMER_INTR_IRQN                  = LOCAL_INTERRUPT0 + 87,
    MJPGE_INTR_IRQN                      = LOCAL_INTERRUPT0 + 88,
    VICAP_INTR_IRQN                      = LOCAL_INTERRUPT0 + 89,

    BUTT_IRQN
} core_irq_t;

#endif

/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2019. All rights reserved.
 * Description:  Application Core Platform Definitions
 *
 * Date:
 */

#ifndef PLATFORM_CORE_H
#define PLATFORM_CORE_H

#include "product.h"
#include "product_pin.h"
#include "soc_reg.h"
#include "chip_core_definition.h"
#include "platform_section.h"

/**
 * @defgroup DRIVER_PLATFORM_CORE CHIP Platform CORE Driver
 * @ingroup DRIVER_PLATFORM
 * @{
 */

#define ULP_AON_CTL_RB_ADDR      0x5702c000
#define FUSE_CTL_RB_ADDR         0x57028000
#define XO_CORE_TRIM_REG         0x57028308
#define XO_CORE_CTRIM_REG        0x5702830c
#define NMI_CTL_REG_BASE_ADDR    0x52000700
#define DIAG_BASE_ADDR           0x52004000

#define UART0_BASE               0x52009000  /* UART H0 */
#define UART1_BASE               0x5200A000  /* UART H1 */
#define UART2_BASE               0x5200B000  /* UART L0 */
#define UART3_BASE               0x55004000  /* UART COM */
#define UART4_BASE               0x59010000  /* UART BT COEX */
#define DMA_BASE_ADDR            0x52070000  /* M_DMA */
#define SDMA_BASE_ADDR           0x52071000  // use for cs chip sdma

/* PWM reg base addr */
#define PWM_BASE_ADDR            0x52015000
#define AON_PWM_BASE_ADDR        0x57021000

// GPIO regs
#define GPIO0_BASE_ADDR          0x52016000
#define GPIO1_BASE_ADDR          0x52017000
#define GPIO2_BASE_ADDR          0x57010000
#define GPIO3_BASE_ADDR          0x57011000

#define ULP_GPIO_BASE_ADDR       0x57019000 // ULP GPIO

// GPIO select core
#define GPIO_BT_CORE_SEL_ADDR          0x57000170
#define GPIO_DSP_CORE_SEL_ADDR         0x57000178
#define GPIO_INVALID_CORE_SEL_ADDR     0xFFFFFFFF

// ULP GPIO int clk config
#define HAL_GPIO_ULP_AON_GP_REG                 0x5702C024
#define HAL_GPIO_ULP_AON_PCLK_INT_EN_BIT        0
#define HAL_GPIO_ULP_AON_PCLK_INT_CLK_SEL_BIT   1
#define HAL_GPIO_ULP_PCLK_INTR_STATUS_BITS      0x3

#define RTC_0_BASE_ADDR                         (GLB_RTC_BASE_ADDR + 0x100)
#define SYSTICK_BASE_ADDR                       0x52000414

#define TCXO_COUNT_BASE_ADDR 0x52000c34

#define CHIP_WDT_BASE_ADDRESS                   0x57020000
#define WDT_CLK_EN                              0x57000168
#define WDT_CLK_EN_BIT                          0
#define WDT_NMI_EN_REG                          M_CTL_RB_M_NMI_INT_EN_REG
#define WDT_NMI_EN_BIT                          M_CTL_RB_CWDT_INT_EN_OFFSET
#define HAL_SOFT_RST_CTL_BASE                   (GLB_CTL_M_RB_BASE)
#define HAL_GLB_CTL_M_ATOP1_L_REG_OFFSET        0x168
#define HAL_CHIP_WDT_ATOP1_RST_BIT              2
#define BT_WDT_BASE_ADDR                        0x59007000

// Timer reg base addr.
#define TIMER_BASE_ADDR                         0x52003000
#define TIMER_0_BASE_ADDR                       (TIMER_BASE_ADDR + 0x100)
#define TIMER_1_BASE_ADDR                       (TIMER_BASE_ADDR + 0x200)
#define TIMER_2_BASE_ADDR                       (TIMER_BASE_ADDR + 0x300)
#define TIMER_3_BASE_ADDR                       (TIMER_BASE_ADDR + 0x400)
#define TIMER_SYSCTL_BASE_ADDR                  (TIMER_BASE_ADDR + 0x500)

#define TICK_TIMER_BASE_ADDR                    TIMER_0_BASE_ADDR

// SDIO HOST
#define SDIO_HOST_BASE_ADDR                     0x52060000
#define EMMC_BASE_ADDR                          0x52061000
#define FMC_CTL_BASE_ADDR                       0x52062000

#define MMC_SUM_NUM             2
#define MMC_NUM0_SDIO           0
#define MMC_NUM1_EMMC           1

// FPGA Ver
#define FPGA_VERSION_REG        0x52000b0c
#define FPGA_VERSION_MASK   0xF
#define FPGA_VER_START_BIT  0
#define FPGA_VER_BIT_LEN    4
#define FPGA_MCU_ONLY       0x1
#define FPGA_MCU_BT         0x2
#define FPGA_MCU_DSP        0x3
#define FPGA_MCU_PSRAM      0x3
#define FPGA_MCU_NPU        0x4
#define FPGA_MCU_DISPLAY    0x5
#define FPGA_MCU_USB        0x6
#define FPGA_MCU_BT_DSP     0x7
#define FPGA_MCU_ALL_CORS   0x8
#define FPGA_EMMC_BIT       (1 << 7) // bit7
#define FPGA_PSRAM_BIT      (1 << 6) // bit6
#define FPGA_DSP_L2M_BIT    (1 << 8) // bit8
#define FPGA_SDIO_BIT       (1 << 9) // bit9

/*
 * Maximum UART buses
 * Defined here rather than in the uart_bus_t enum, due to needing to use it for conditional compilation
 */
#define UART_BUS_MAX_NUMBER 4  // !< Max number of UARTS available
#define PM_RESUME_UART_BUS_NUM  UART_BUS_MAX_NUMBER

#define TIMER_MAX_AVAILABLE_NUMBER    4  // !< Max number of timer available
#define RTC_MAX_AVAILABLE_NUMBER      4  // !< Max number of rtc available

#define GPIO_MAX_NUMBER     5  // !< Max number of GPIO available
#define PWM_MAX_NUMBER      6  // !< Max number of PWM available

#define S_DMA_CHANNEL_MAX_NUM    4  // !< Max number of SM_DMA available
#define B_DMA_CHANNEL_MAX_NUM    8  // !< Max number of M_DMA available

#define DMA_CHANNEL_MAX_NUM      (S_DMA_CHANNEL_MAX_NUM + B_DMA_CHANNEL_MAX_NUM)

#define CHIP_BCPU_SWDDIO  0
#define CHIP_BCPU_SWDCLK  0


#define HOST_BURN_UART_BUS      UART_BUS_1
#define HOST_BURN_UART_TX_PIN   S_MGPIO19
#define HOST_BURN_UART_RX_PIN   S_MGPIO20

#define TEST_SUITE_UART_BUS     UART_BUS_2
#define TEST_SUITE_UART_TX_PIN  S_MGPIO17
#define TEST_SUITE_UART_RX_PIN  S_MGPIO18

#define CODELOADER_UART_BUS     UART_BUS_2
#define CODELOADER_UART_BASE    UART2_BASE
#define CODELOADER_UART_TX_PIN  S_MGPIO17
#define CODELOADER_UART_RX_PIN  S_MGPIO18

#define AT_UART_BUS             UART_BUS_2
#define AT_UART_TX_PIN          S_MGPIO17
#define AT_UART_RX_PIN          S_MGPIO18
#ifdef CONFIG_AT_UART_BAUDRATE
#define AT_UART_BAUD_RATE       CONFIG_AT_UART_BAUDRATE
#else
#define AT_UART_BAUD_RATE       115200
#endif

#ifdef FT_SINGLE_UART
#define LOG_UART_BUS            UART_BUS_2
#define LOG_UART_TX_PIN         S_MGPIO17
#define LOG_UART_RX_PIN         S_MGPIO18
#else
#define LOG_UART_BUS            UART_BUS_3
#define LOG_UART_TX_PIN         S_AGPIO7
#define LOG_UART_RX_PIN         S_AGPIO8
#endif

#define SW_DEBUG_UART_BUS       UART_BUS_2
#define CHIP_FIXED_TX_PIN       S_MGPIO17
#define CHIP_FIXED_RX_PIN       S_MGPIO18

/**
 * @brief  Definition of UART bus index.
 */
typedef enum {
    UART_BUS_0 = 0,  // !< UART1
#if UART_BUS_MAX_NUMBER > 1
    UART_BUS_1 = 1,  // !< HS_UART
#endif
#if UART_BUS_MAX_NUMBER > 2
    UART_BUS_2 = 2,
#endif
#if UART_BUS_MAX_NUMBER > 3
    UART_BUS_3 = 3,
#endif
    UART_BUS_NONE = UART_BUS_MAX_NUMBER  // !< Value used as invalid/unused UART number
} uart_bus_t;

// UART_BUS_X和DMA握手号映射关系
#define DMA_HANDSHAKE_UART_BUS_0_TX      HAL_DMA_HANDSHAKING_UART_H0_TX
#define DMA_HANDSHAKE_UART_BUS_0_RX      HAL_DMA_HANDSHAKING_UART_H0_RX
#define DMA_HANDSHAKE_UART_BUS_1_TX      HAL_DMA_HANDSHAKING_UART_H1_TX
#define DMA_HANDSHAKE_UART_BUS_1_RX      HAL_DMA_HANDSHAKING_UART_H1_RX
#define DMA_HANDSHAKE_UART_BUS_2_TX      HAL_DMA_HANDSHAKING_UART_L0_TX
#define DMA_HANDSHAKE_UART_BUS_2_RX      HAL_DMA_HANDSHAKING_UART_L0_RX

#define I2C_BUS_MAX_NUM  4  // !< Max number of I2C available
/**
 * @brief  Definition of I2C bus index.
 */
typedef enum {
    I2C_BUS_0,
    I2C_BUS_1,
    I2C_BUS_2,
    I2C_BUS_3,
    I2C_BUS_NONE = I2C_BUS_MAX_NUM
} i2c_bus_t;
// I2C_BUS_X和基地址映射关系
#define I2C_BUS_0_BASE_ADDR             0x5200C000 // 设计信息表单I2C0
#define I2C_BUS_1_BASE_ADDR             0x5200D000 // 设计信息表单I2C1
#define I2C_BUS_2_BASE_ADDR             0x5200E000 // 设计信息表单I2C2
#define I2C_BUS_3_BASE_ADDR             0x5200F000 // 设计信息表单I2C3
// I2C_BUS_X和中断号映射关系
#define I2C_BUS_0_IRQN                  I2C_0_IRQN
#define I2C_BUS_1_IRQN                  I2C_1_IRQN
#define I2C_BUS_2_IRQN                  I2C_2_IRQN
#define I2C_BUS_3_IRQN                  I2C_3_IRQN

// I2C_BUS_X和DMA握手号映射关系
#define DMA_HANDSHAKE_I2C_BUS_0_TX      HAL_DMA_HANDSHAKING_I2C0_TX
#define DMA_HANDSHAKE_I2C_BUS_0_RX      HAL_DMA_HANDSHAKING_I2C0_RX
#define DMA_HANDSHAKE_I2C_BUS_1_TX      HAL_DMA_HANDSHAKING_I2C1_TX
#define DMA_HANDSHAKE_I2C_BUS_1_RX      HAL_DMA_HANDSHAKING_I2C1_RX
#define DMA_HANDSHAKE_I2C_BUS_2_TX      HAL_DMA_HANDSHAKING_I2C2_TX
#define DMA_HANDSHAKE_I2C_BUS_2_RX      HAL_DMA_HANDSHAKING_I2C2_RX
#define DMA_HANDSHAKE_I2C_BUS_3_TX      HAL_DMA_HANDSHAKING_I2C4_TX
#define DMA_HANDSHAKE_I2C_BUS_3_RX      HAL_DMA_HANDSHAKING_I2C4_RX

#define SPI_BUS_MAX_NUM  3  // !< Max number of SPI available
/**
 * @brief  Definition of SPI bus index.
 */
typedef enum {
    SPI_BUS_0 = 0,
    SPI_BUS_1,
    SPI_BUS_2,          /* QSPI */
    SPI_BUS_NONE = SPI_BUS_MAX_NUM
} spi_bus_t;
// SPI_BUS_X和基地址映射关系
#define SPI_BUS_0_BASE_ADDR             0x52011000  // 设计信息表单 SPI1_MS
#define SPI_BUS_1_BASE_ADDR             0x52012000  // 设计信息表单 SPI2_MS
#define SPI_BUS_2_BASE_ADDR             0x24000000  // 设计信息表单 QSPI2_1CS
// SPI_BUS_X和中断号映射关系
#define SPI_BUS_0_IRQN                  SPI1_MS_IRQN
#define SPI_BUS_1_IRQN                  SPI2_MS_IRQN
#define SPI_BUS_2_IRQN                  QSPI2_CS_IRQN
// SPI_BUS_X和DMA握手号映射关系
#define DMA_HANDSHAKE_SPI_BUS_0_TX      HAL_DMA_HANDSHAKING_SPI0_MS_TX
#define DMA_HANDSHAKE_SPI_BUS_0_RX      HAL_DMA_HANDSHAKING_SPI0_MS_RX
#define DMA_HANDSHAKE_SPI_BUS_1_TX      HAL_DMA_HANDSHAKING_SPI1_MS_TX
#define DMA_HANDSHAKE_SPI_BUS_1_RX      HAL_DMA_HANDSHAKING_SPI1_MS_RX
#define DMA_HANDSHAKE_SPI_BUS_2_TX      HAL_DMA_HANDSHAKING_QSPI2_1CS_TX
#define DMA_HANDSHAKE_SPI_BUS_2_RX      HAL_DMA_HANDSHAKING_QSPI2_1CS_RX


/* !< SLAVE CPU */
typedef enum {
    SLAVE_CPU_DSP0,
    SLAVE_CPU_BT,
    SLAVE_CPU_MAX_NUM,
} slave_cpu_t;

#define DISPLAY_RAM1            0x20340000
#define DISPLAY_RAM2            0x22000000

// CHIP RESET offset address
#define CHIP_RESET_OFF   0x204
#define PIN_MAX_NUMBER                    PIN_NONE // value USED to iterate in arrays
#define GPIO_FUNC                         HAL_PIO_FUNC_GPIO

#define I2C_AUTO_SEND_STOP_CMD            NO
#define I2C_WITH_BUS_RECOVERY             YES

#define TRACE_MEM_REGION_START            MCPU_TRACE_MEM_REGION_START
#define TRACE_MEM_REGION_LENGTH           CPU_TRACE_MEM_REGION_LENGTH

#define IS_MAIN_CORE                      YES

#define SUPPORT_HI_EMMC_PHY               NO

#define USE_SDIOM_CLK_CONFIG_WAY          2
#define CRITICAL_INT_RESTORE              YES

#define CONFIG_MMC0_CCLK_MAX             32000000         // 32MHz
#define CONFIG_MMC1_CCLK_MAX             32000000         // 32MHz
#define SDIO_EMMC_DIV_NUM_MASK           0xFF0

#define SUPPORT_PARTITION_FEATURE        YES

/**
 * @}
 */
#endif

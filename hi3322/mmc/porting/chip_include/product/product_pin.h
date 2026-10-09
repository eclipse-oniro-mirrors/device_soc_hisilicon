/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2020-2020. All rights reserved.
 * Description:  3322 product config
 * Author: @CompanyNameTag
 * Create:  2020-10-23
 */
#ifndef PRODUCT_PIN_H
#define PRODUCT_PIN_H

/**
 * @addtogroup PRODUCT
 * @{
 */

/**
 * @brief Definition of pin.
 */
typedef enum {
    S_EGPIO0 = 0,   // QSPI0_CLK
    S_EGPIO1 = 1,   // QSPI0_CS
    S_EGPIO2 = 2,   // QSPI0_D0
    S_EGPIO3 = 3,   // QSPI0_D1
    S_EGPIO4 = 4,   // QSPI0_D2
    S_EGPIO5 = 5,   // QSPI0_D3
    S_EGPIO6 = 6,   // QSPI3_CLK
    S_EGPIO7 = 7,   // QSPI3_CS
    S_EGPIO8 = 8,   // QSPI3_D0
    S_EGPIO9 = 9,   // QSPI3_D1
    S_EGPIO10 = 10, // QSPI3_D2
    S_EGPIO11 = 11, // QSPI3_D3

    S_EGPIO12 = 12, //
    S_EGPIO13 = 13, //
    S_EGPIO14 = 14, //
    S_EGPIO15 = 15, //
    S_EGPIO16 = 16, //
    S_EGPIO17 = 17, //
    S_EGPIO18 = 18, //
    S_EGPIO19 = 19, //
    S_EGPIO20 = 20, //
    S_EGPIO21 = 21, //
    S_EGPIO22 = 22, //
    S_EGPIO23 = 23, //
    S_EGPIO24 = 24, //
    S_EGPIO25 = 25, //
    S_EGPIO26 = 26, //
    S_EGPIO27 = 27, //
    S_EGPIO28 = 28, //
    S_EGPIO29 = 29, //
    S_EGPIO30 = 30, //
    S_EGPIO31 = 31, //

    S_MGPIO0 = 32,  // UART0_TXD
    S_MGPIO1 = 33,  // UART0_RXD
    S_MGPIO2 = 34,  // UART0_CTS
    S_MGPIO3 = 35,  // UART0_RTS
    S_MGPIO4 = 36,  // I2C0_SCL
    S_MGPIO5 = 37,  // I2C0_SDA
    S_MGPIO6 = 38,  // I2C1_SCL
    S_MGPIO7 = 39,  // I2C1_SDA
    S_MGPIO8 = 40,  // I2C2_SCL
    S_MGPIO9 = 41,  // I2C2_SDA
    S_MGPIO10 = 42, // I2C3_SCL
    S_MGPIO11 = 43, // I2C3_SDA
    S_MGPIO12 = 44, // SPI0_DI
    S_MGPIO13 = 45, // SPI0_DO
    S_MGPIO14 = 46, // SPI0_CLK
    S_MGPIO15 = 47, // SPI0_CS
    S_MGPIO16 = 48, // SPI0_CS1
    S_MGPIO17 = 49, // UART2_TXD
    S_MGPIO18 = 50, // UART2_RXD
    S_MGPIO19 = 51, // UART1_TXD
    S_MGPIO20 = 52, // UART1_RXD
    S_MGPIO21 = 53, // UART1_CTS
    S_MGPIO22 = 54, // UART1_RTS
    S_MGPIO23 = 55, // SPI1_DI
    S_MGPIO24 = 56, // SPI1_DO
    S_MGPIO25 = 57, // SPI1_CLK
    S_MGPIO26 = 58, // SPI1_CS

    S_HGPIO0 = 59,  // EMMC_D0
    S_HGPIO1 = 60,  // EMMC_D1
    S_HGPIO2 = 61,  // EMMC_D2
    S_HGPIO3 = 62,  // EMMC_D3
    S_HGPIO4 = 63,  // EMMC_D4
    S_HGPIO5 = 64,  // EMMC_D5
    S_HGPIO6 = 65,  // EMMC_D6
    S_HGPIO7 = 66,  // EMMC_D7
    S_HGPIO8 = 67,  // EMMC_CMD
    S_HGPIO9 = 68,  // EMMC_CLK
    S_HGPIO10 = 69, // SDIO_D0
    S_HGPIO11 = 70, // SDIO_D1
    S_HGPIO12 = 71, // SDIO_D2
    S_HGPIO13 = 72, // SDIO_D3
    S_HGPIO14 = 73, // SDIO_CLK
    S_HGPIO15 = 74, // SDIO_CMD

    S_AGPIO0 = 75,   //
    S_AGPIO1 = 76,   //
    S_AGPIO2 = 77,   //
    S_AGPIO3 = 78,   //
    S_AGPIO4 = 79,   //
    S_AGPIO5 = 80,   //
    S_AGPIO6 = 81,   //
    S_AGPIO7 = 82,   //
    S_AGPIO8 = 83,   //
    S_AGPIO9 = 84,   //
    S_AGPIO10 = 85,  //
    S_AGPIO11 = 86,  //
    S_AGPIO12 = 87,  //
    S_AGPIO13 = 88,  //
    S_AGPIO14 = 89,  //
    S_AGPIO15 = 90,  //
    S_AGPIO16 = 91,  //
    S_AGPIO17 = 92,  //
    S_AGPIO18 = 93,  //
    S_AGPIO19 = 94,  //
    S_AGPIO20 = 95,  //
    S_AGPIO21 = 96,  //
    S_AGPIO22 = 97,  //
    S_AGPIO23 = 98,  //
    S_AGPIO24 = 99,  //
    S_AGPIO25 = 100, //
    S_AGPIO26 = 101, //
    S_AGPIO27 = 102, //
    S_AGPIO28 = 103, //
    S_AGPIO29 = 104, //
    S_AGPIO30 = 105, //
    S_AGPIO31 = 106, //
    S_AGPIO32 = 107, //
    S_AGPIO33 = 108, //
    S_AGPIO34 = 109, //

    S_UGPIO0 = 110, //
    S_UGPIO1 = 111, //

    PIN_NONE,
} pin_t;

/**
 * @}
 */

#endif

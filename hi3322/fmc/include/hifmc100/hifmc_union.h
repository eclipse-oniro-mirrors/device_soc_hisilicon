/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2024-2024. All rights reserved.
 */

#ifndef __HIFMC_UNION_H__
#define __HIFMC_UNION_H__

/* Define the union u_dmac_g_en */
typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    op_mode               : 1   ; /* [0]       */
        uint32_t    flash_sel             : 2   ; /* [2..1]    */
        uint32_t    page_size             : 2   ; /* [4..3]    */
        uint32_t    ecc_type              : 3   ; /* [7..5]    */
        uint32_t    block_size            : 2   ; /* [9..8]    */
        uint32_t    spi_nor_addr_mode     : 1   ; /* [10]      */
        uint32_t    spi_nand_sel          : 2   ; /* [12..11]  */
        uint32_t    reserved              : 19  ; /* [31..13]  */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_cfg; /* 0x0 */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    cmd1                  : 8   ; /* [7..0]     */
        uint32_t    cmd2                  : 8   ; /* [15..8]    */
        uint32_t    reserved              : 16  ; /* [31..16]   */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_cmd; /* 0x24 */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    addrh                 : 8   ; /* [7..0]     */
        uint32_t    reserved              : 24  ; /* [31..8]   */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_addrh; /* 0x28 */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    addrl                 : 32   ; /* [31..0]     */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_addrl; /* 0x2C */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    dummy_num             : 4   ; /* [3..0]     */
        uint32_t    addr_num              : 3   ; /* [6..4]    */
        uint32_t    mem_if_type           : 3   ; /* [9..7]    */
        uint32_t    force_cs_en           : 1   ; /* [10]    */
        uint32_t    fm_cs                 : 1   ; /* [11]    */
        uint32_t    reserved              : 20  ; /* [31..12]   */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_op_cfg; /* 0x30 */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    op_data_num           : 14  ; /* [13..0]     */
        uint32_t    reserved              : 18  ; /* [31..14]    */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_data_num; /* 0x38 */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    reg_op_start          : 1   ; /* [0]     */
        uint32_t    read_status_en        : 1   ; /* [1]    */
        uint32_t    read_data_en          : 1   ; /* [2]    */
        uint32_t    wait_ready_en         : 1   ; /* [3]    */
        uint32_t    cmd2_en               : 1   ; /* [4]    */
        uint32_t    write_data_en         : 1   ; /* [5]    */
        uint32_t    addr_en               : 1   ; /* [6]    */
        uint32_t    cmd1_en               : 1   ; /* [7]    */
        uint32_t    dummy_en              : 1   ; /* [8]    */
        uint32_t    reserved              : 20  ; /* [31..9]   */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_op; /* 0x3C */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    dma_len               : 28  ; /* [27..0]     */
        uint32_t    reserved              : 4   ; /* [31..28]    */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_dma_len; /* 0x40 */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    dma_mem_saddr_d0      : 32   ; /* [31..0]     */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_dma_saddr_d0; /* 0x4C */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    dma_mem_saddr_oob     : 32   ; /* [31..0]     */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_dma_saddr_oob; /* 0x5C */

typedef union {
    /* Define the struct bits */
    struct {
        uint32_t    dma_op_ready          : 1   ; /* [0]      */
        uint32_t    rw_op                 : 1   ; /* [1]      */
        uint32_t    reserved              : 2   ; /* [3..2]   */
        uint32_t    rd_op_sel             : 2   ; /* [5..4]      */
        uint32_t    reserved1              : 2   ; /* [7..6]    */
        uint32_t    wr_opcode             : 8   ; /* [15..8]    */
        uint32_t    rd_opcode             : 8   ; /* [23..16]    */
        uint32_t    reserved2              : 8  ; /* [31..24]   */
    } bits;

    /* Define an unsigned member */
    uint32_t    value;
} reg_fmc_op_ctrl; /* 0x68 */

#endif /* __HIFMC_UNION_H__ */
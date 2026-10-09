/* ----------------------------------------------------------------------------
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description: LiteOS Board.
 *
 * Create: 2021-09-03
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --------------------------------------------------------------------------- */

#include <sys/mount.h>
#include "soc/mmc.h"
#include "mmc_porting.h"
#include "block.h"
#include "disk.h"
#include "host.h"
#include "chip_io.h"
#include "dpal_driver.h"
#include "fs/fs.h"
#include "soc_reg.h"
#include "proc_fs.h"
#include "product.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#define MMC_SECTOR_SIZE	            512
#define EMMC_PINMUX_NUM             10
#define EMMC_PAD_PINMUX_NUM         9
#define SDIO_PINMUX_NUM             6
#define SDIO_PINMUX_PAD_NUM         5
#define EMMC_NUM100                 100
#define EMMC_PART_MODE              0666
#define EMMC_MSLEEP_20              20
#define EMMC_ACCESS_OFFSET          20
#define SDIO_CKG_CTRL_AND_STS       0x124
#define SDIO_CFG_CKG_BYP_SDHCR_POS  23

#define REG_ADDR_LEN                4
#define SDIO_PIN_FUN                1

#define FF_VOLUMES_NUM              5
#define FF_VOLUME_NAME_BOOT          "/boot/"
#define FF_VOLUME_NAME_SYS          "/system/"
#define FF_VOLUME_NAME_INNER        "/inner/"
#define FF_VOLUME_NAME_SYS_BK       "/systembk/"
#define FF_VOLUME_NAME_USER         "/user/"

struct ff_volume_info {
    const char *name;
    uint64_t sector_count;
};

static struct ff_volume_info g_ff_volume_infos [FF_VOLUMES_NUM] = {
    {FF_VOLUME_NAME_BOOT, EMMC_VOLUME_BOOT_SECTOR_COUNT},
    {FF_VOLUME_NAME_SYS, EMMC_VOLUME_SYS_SECTOR_COUNT},
    {FF_VOLUME_NAME_INNER, EMMC_VOLUME_INNER_SECTOR_COUNT},
    {FF_VOLUME_NAME_SYS_BK, EMMC_VOLUME_SYS_BK_SECTOR_COUNT},
    {FF_VOLUME_NAME_USER, 0xffffffff} // 运行时修正为剩余扇区数
};

static struct dpal_resource sdmmc0_resources[] = {
    {
        .start  = EMMC_REG_BASE_ADDESS,
        .end    = EMMC_REG_END_ADDRESS,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = EMMC_INTERRUPT_IRQ,
        .end    = EMMC_INTERRUPT_IRQ,
        .flags  = IORESOURCE_IRQ,
    },
    {
        .start  = PERI_EMMC_CRG_RST_CTL,
        .end    = PERI_EMMC_CRG_RST_CTL,
        .flags  = IORESOURCE_REG,
    },
};

struct dpal_platform_device device_sdmmc0 = {
    .name       = "sdhci0",
    .id     = 0,
    .resource   = sdmmc0_resources,
    .num_resources  = sizeof(sdmmc0_resources) / sizeof(sdmmc0_resources[0]),
};

static struct dpal_resource sdmmc1_resources[] = {
    {
        .start  = SDIO_HOST_REG_BASE_ADDESS,
        .end    = SDIO_HOST_REG_END_ADDRESS,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = SDIO_INTERRUPT_IRQ,
        .end    = SDIO_INTERRUPT_IRQ,
        .flags  = IORESOURCE_IRQ,
    },
    {
        .start  = PERI_EMMC_CRG_RST_CTL,
        .end    = PERI_EMMC_CRG_RST_CTL,
        .flags  = IORESOURCE_REG,
    },
};

struct dpal_platform_device device_sdmmc1 = {
    .name       = "sdhci1",
    .id     = 1,
    .resource   = sdmmc1_resources,
    .num_resources  = sizeof(sdmmc1_resources) / sizeof(sdmmc1_resources[0]),
};

static bool g_mmc_inited = false;
static bool g_sdio_inited = false;

static int32_t emmc_disk_partition_cfg(uint64_t emmc_capacity)
{
    int64_t total_sector_num = (int64_t)emmc_capacity - RESERVED_BOOT_SECTOR;
    int64_t user_sector_count = total_sector_num -
        (EMMC_VOLUME_BOOT_SECTOR_COUNT + EMMC_VOLUME_SYS_SECTOR_COUNT +
         EMMC_VOLUME_INNER_SECTOR_COUNT + EMMC_VOLUME_SYS_BK_SECTOR_COUNT);
    if (total_sector_num <= 0 || user_sector_count <= 0) {
        return -EINVAL;
    }
    int ret;
    g_ff_volume_infos[FF_VOLUMES_NUM - 1].sector_count = user_sector_count;
    size_t part_start_sector = RESERVED_BOOT_SECTOR;
    size_t part_count_sector;
    for (int index = 0; index < FF_VOLUMES_NUM; ++index) {
        part_count_sector = g_ff_volume_infos[index].sector_count;
        ret = add_mmc_partition(&emmc, part_start_sector, part_count_sector);
        if (ret != 0) {
            return -EIO;
        }
        part_start_sector += part_count_sector;
    }
    return ENOERR;
}

static void switch_emmc_router(uint32_t freq_level)
{
    clocks_set_emmc_freq(freq_level);
}

int emmc_drv_init(void)
{
#define LOW_SECTORS_PER_BLOCK 32 // 跟Bcache大小有关系，当前设置32 最终的出来的bcache大小是32K左右
    emmc_partition_ruler_register(emmc_disk_partition_cfg);
    emmc_couter_config_register(switch_emmc_router);
    (void)platform_device_register(&device_sdmmc0);
    proc_fs_init();
    SetFatSectorsPerBlock(LOW_SECTORS_PER_BLOCK);
    int ret = MMC_HostInitById(0);
    if (ret != 0) {
        return -EIO;
    }
    return ENOERR;
}

int emmc_drv_try_mount(void)
{
    int ret;
    int try_access_num = 0;
    char dev_name[64] = { 0 };

    for (uint32_t index = 0; index < FF_VOLUMES_NUM; ++index) {
        ret = sprintf_s(dev_name, sizeof(dev_name), "/dev/mmcblk0p%u", index);
        if (ret <= 0) {
            return -EIO;
        }

        do {
            ret = los_part_access(dev_name, EMMC_PART_MODE);
            ++try_access_num;
            uapi_tcxo_delay_ms(EMMC_MSLEEP_20);
        } while ((ret != ENOERR) && (try_access_num < EMMC_ACCESS_OFFSET));

        if (ret != ENOERR) {
            return -EACCES;
        }

        ret = mount(dev_name, g_ff_volume_infos[index].name, "vfat", 0, NULL);
        if (ret != OK) {
            printf("%s, mount failed, partition:[%u]\r\n", __FUNCTION__, index);
            return -ENODEV;
        }
    }

    return ENOERR;
}

int emmc_drv_format(void)
{
    int ret;
    char dev_name[64] = { 0 };
    for (uint32_t index = 0; index < FF_VOLUMES_NUM; ++index) {
        ret = sprintf_s(dev_name, sizeof(dev_name), "/dev/mmcblk0p%u", index);
        if (ret <= 0) {
            continue;
        }
        umount(g_ff_volume_infos[index].name);
        ret = format(dev_name, SECTOR_NUM_PER_CLUSTER, FMT_FAT32 | FMT_ERASE);
        if (ret != 0) {
            return -EIO;
        }
        ret = mount(dev_name, g_ff_volume_infos[index].name, "vfat", 0, NULL);
        if (ret != OK) {
            return -ENODEV;
        }
    }
    return ENOERR;
}

int emmc_drv_context_init(void)
{
    int ret;
    pre_timer_init();  // kernel调度前使用timer, 保证emmc在kernel初始化前正常使用
    ret = emmc_drv_init();
    if (ret != ENOERR) {
        printf("[%s:%d] ret[%d]\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    g_mmc_inited = true;
    /* 卡时钟门控配置，器件初始化后才能配置 */
    writel(EMMC_BASE_ADDR + CKG_CTRL_AND_STS, 0x2000000);

    ret = emmc_drv_try_mount();
    if (ret == ENOERR) {
        printf("[%s:%d] ret[%d]\n", __FUNCTION__, __LINE__, ret);
        return ret;
    }

    return emmc_drv_format();
}

int sdio_drv_context_init(void)
{
    (void)platform_device_register(&device_sdmmc1);
    int ret = MMC_HostInitById(1);
    if (ret != 0) {
        printf("MMC_HostInitById init sdio error, ret = %d\r\n", ret);
    }
    g_sdio_inited = true;
    /* 卡时钟门控配置，器件初始化后才能配置 */
    writel(SDIO_HOST_BASE_ADDR + CKG_CTRL_AND_STS, 0x2000000);
    return ret;
}

typedef struct {
    uint16_t host_clk_div;
    uint16_t block_size; // 0x4
    uint32_t host_ctrl_blk_gap_ctrl; // 0x28
    uint32_t clock_ctrl; // 0x2c
    uint32_t int_stat_enable; // 0x34
    uint32_t int_signal_enable; // 0x38
    uint32_t mmc_rx_tune_ctl;
    uint32_t mmc_card_tune_ctl;
    uint32_t ckg_ctrl_and_sts; // 0x124
} mmc_save_info_t;

static mmc_save_info_t g_mmc_cur_info[MMC_SUM_NUM] = { 0 };

mmc_save_info_t* sdhci_get_save_info(uint8_t mmc_type)
{
    return &g_mmc_cur_info[mmc_type];
}

static void mmc_save_info(uint8_t mmc_type)
{
    uint32_t base_addr = 0;
    uint32_t clk_addr = 0;
    mmc_save_info_t *ptr = NULL;

    // ToDo:save tuning
    if (mmc_type == MMC_NUM0_SDIO) {
        base_addr = SDIO_HOST_BASE_ADDR;
        clk_addr = HS_CTL_RB_SD_HOST_CCLK_DIV_REG;
        ptr = &g_mmc_cur_info[MMC_NUM0_SDIO];
    } else {
        base_addr = EMMC_BASE_ADDR;
        clk_addr = HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG;
        ptr = &g_mmc_cur_info[MMC_NUM1_EMMC];
    }
    if (mmc_type == MMC_NUM1_EMMC) {
        ptr->mmc_card_tune_ctl = readl(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG);
        ptr->mmc_rx_tune_ctl = readl(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG);
    }
    ptr->host_clk_div = readw(clk_addr);
    ptr->block_size = readw(base_addr + BLOCK_SIZE); // 0x4
    ptr->host_ctrl_blk_gap_ctrl = readl(base_addr + HOST_CONTROL_BLOCK_GAP_CONTROL); // 0x28
    ptr->clock_ctrl = readl(base_addr + CLOCK_CONTROL); // 0x2c
    ptr->int_stat_enable = readl(base_addr + INTERRUPT_STATUS_ENABLE); // 0x34
    ptr->int_signal_enable = readl(base_addr + INTERRUPT_SIGNAL_ENABLE); // 0x38
    ptr->ckg_ctrl_and_sts = readl(base_addr + CKG_CTRL_AND_STS); // 0x124
}

static void mmc_recovery_init(uint8_t mmc_type)
{
    uint32_t base_addr = 0;
    uint32_t clk_addr = 0;
    mmc_save_info_t *ptr = NULL;

    // ToDo:recovery tuning
    if (mmc_type == MMC_NUM0_SDIO) {
        base_addr = SDIO_HOST_BASE_ADDR;
        clk_addr = HS_CTL_RB_SD_HOST_CCLK_DIV_REG;
        ptr = &g_mmc_cur_info[MMC_NUM0_SDIO];
        reg16_setbit(HS_CTL_RB_HS_CLK_EN_REG, HS_CTL_RB_CFG_HS_SDIO_CLKEN_OFFSET);
        reg32_setbit(SDIO_HOST_REG_BASE_ADDESS + SDIO_CKG_CTRL_AND_STS, SDIO_CFG_CKG_BYP_SDHCR_POS);
    } else {
        base_addr = EMMC_BASE_ADDR;
        clk_addr = HS_CTL_RB_EMMC_HOST_CCLK_DIV_REG;
        ptr = &g_mmc_cur_info[MMC_NUM1_EMMC];
    }
    if (mmc_type == MMC_NUM1_EMMC) {
        writel(HS_CTL_RB_EMMC_TUNING_CARD_CLK_CTL_REG, ptr->mmc_card_tune_ctl);
        writel(HS_CTL_RB_EMMC_TUNING_CCLK_RX_CTL_REG, ptr->mmc_rx_tune_ctl);
    }
    writew(clk_addr, ptr->host_clk_div);
    writel(base_addr + CLOCK_CONTROL, ptr->clock_ctrl); // 0x2C
    writew(base_addr + BLOCK_SIZE, ptr->block_size); // 0x4
    writel(base_addr + HOST_CONTROL_BLOCK_GAP_CONTROL, ptr->host_ctrl_blk_gap_ctrl); // 0x28
    writel(base_addr + INTERRUPT_STATUS_ENABLE, ptr->int_stat_enable); // 0x34
    writel(base_addr + INTERRUPT_SIGNAL_ENABLE, ptr->int_signal_enable); // 0x38
    writel(base_addr + CKG_CTRL_AND_STS, ptr->ckg_ctrl_and_sts); // 0x124
}

void emmc_recovery_init(void)
{
    if (g_mmc_inited) {
        for (uint32_t i = 0; i < EMMC_PINMUX_NUM; i++) {
            writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO0_MODE_REG + i * REG_ADDR_LEN, SDIO_PIN_FUN);
        }
        mmc_recovery_init(MMC_NUM1_EMMC);
    }
}

void emmc_save_info(void)
{
    if (g_mmc_inited) {
        mmc_save_info(MMC_NUM1_EMMC);
    }
}

void emmc_set_clock(uint32_t clock_value)
{
    if (g_mmc_inited) {
        hi_set_emmc_clock(clock_value);
    }
}

void sdio_recovery_init(void)
{
    if (g_sdio_inited) {
        for (uint32_t i = 0; i < SDIO_PINMUX_NUM; i++) {
            writew(CFG_S_HGPIO_MODE_CFG_S_HGPIO10_MODE_REG + i * REG_ADDR_LEN, SDIO_PIN_FUN);
        }
        mmc_recovery_init(MMC_NUM0_SDIO);
    }
}

void sdio_save_info(void)
{
    if (g_sdio_inited) {
        mmc_save_info(MMC_NUM0_SDIO);
    }
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

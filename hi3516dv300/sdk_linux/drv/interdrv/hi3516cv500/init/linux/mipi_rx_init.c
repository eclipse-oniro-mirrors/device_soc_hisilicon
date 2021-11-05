/*
 * Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/version.h>
#include <linux/of_platform.h>

#include "hi_mipi.h"

extern unsigned int g_mipi_rx_irq_num;
extern void *g_mipi_rx_regs_va;

extern int mipi_rx_mod_init(void);
extern void mipi_rx_mod_exit(void);

static int hi35xx_mipi_rx_probe(struct platform_device *pdev)
{
    struct resource *mem;

    mem = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mipi_rx");
    g_mipi_rx_regs_va = devm_ioremap_resource(&pdev->dev, mem);
    if (IS_ERR(g_mipi_rx_regs_va)) {
        dev_err(&pdev->dev, "mipi_rx remap mem error.\n");
        return PTR_ERR(g_mipi_rx_regs_va);
    }

    g_mipi_rx_irq_num = platform_get_irq_byname(pdev, "mipi_rx");
    if (g_mipi_rx_irq_num <= 0) {
        dev_err(&pdev->dev, "can not find mipi_rx IRQ\n");
    }

    return mipi_rx_mod_init();
}

static int hi35xx_mipi_rx_remove(struct platform_device *pdev)
{
    hi_mipi_rx_unused(pdev);
    mipi_rx_mod_exit();
    g_mipi_rx_regs_va = NULL;

    return 0;
}

static const struct of_device_id g_hi35xx_mipi_rx_match[] = {
    { .compatible = "hisilicon,hisi-mipi" },
    {},
};
MODULE_DEVICE_TABLE(of, g_hi35xx_mipi_rx_match);

static struct platform_driver g_hi35xx_mipi_rx_driver = {
    .probe  = hi35xx_mipi_rx_probe,
    .remove = hi35xx_mipi_rx_remove,
    .driver = {
        .name           = "hi35xx_mipi_rx",
        .of_match_table = g_hi35xx_mipi_rx_match,
    },
};

module_platform_driver(g_hi35xx_mipi_rx_driver);

MODULE_AUTHOR("Digital Media Team ,Hisilicon crop.");
MODULE_DESCRIPTION("Mipi_Rx Driver");
MODULE_VERSION("HI_VERSION = HISI_mipi @HiMPP");
MODULE_LICENSE("GPL");

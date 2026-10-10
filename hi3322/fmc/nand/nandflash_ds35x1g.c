/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2018-2020. All rights reserved.
 * Description:
 *
 * Create:  2020-10-15
 */

#include "securec.h"
#include "debug_print.h"
#include "nandflash_ds35x1g.h"

#define DS35X1G_OOB_LEN 64
void ds35x1g_oob_format(int32_t flag, char *oob_data, int32_t oob_len)
{
    char temp[DS35X1G_OOB_LEN];
    errno_t ret = memcpy_s(temp, DS35X1G_OOB_LEN, oob_data, oob_len);
    if (ret != 0) {
        PRINT("oob_format memcpy ret = 0x%x\n", ret);
    }
    (void)memset_s(oob_data, oob_len, 0xff, oob_len);

    /* nandflash 使能ECC后，OOB区非连续，需要进行转换 */
    const uint8_t indices[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
                                0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37 };
    size_t indices_count = sizeof(indices) / sizeof(indices[0]);

    if (flag) { /* oob write */
        for (size_t i = 0; i < indices_count; i++) {
            oob_data[indices[i]] = temp[i];
        }
    } else { /* oob read */
        for (size_t i = 0; i < indices_count; i++) {
            oob_data[i] = temp[indices[i]];
        }
    }
}

/*
 * Copyright (c) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "os05a_cmos.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HI_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "hi_i2c.h"
#endif

static int g_fd[ISP_MAX_PIPE_NUM] = {[0 ...(ISP_MAX_PIPE_NUM - 1)] = -1};

#define Os05a_5M_2688x1944_12bit_linear30 0
#define Os05a_5M_2688x1944_10bit_wdr30    1
#define I2C_DEV_FILE_NUM     16
#define I2C_BUF_NUM          8
/* VI 297M */
void os05a_init_5M_2688x1944_10bit_wdr30(VI_PIPE vi_pipe);
/* VI 250M */
void os05a_init_5M_2688x1944_12bit_linear30(VI_PIPE vi_pipe);

int os05a_i2c_init(VI_PIPE vi_pipe)
{
    char acDevFile[I2C_DEV_FILE_NUM] = {0};
    HI_U8 u8DevNum;
    ISP_SNS_COMMBUS_U *os05abusinfo = HI_NULL;
    os05abusinfo = os05a_get_bus_Info(vi_pipe);
    if (g_fd[vi_pipe] >= 0) {
        return HI_SUCCESS;
    }
#ifdef HI_GPIO_I2C
    int ret;

    g_fd[vi_pipe] = open("/dev/gpioi2c_ex", O_RDONLY, S_IRUSR);
    if (g_fd[vi_pipe] < 0) {
        SNS_ERR_TRACE("Open gpioi2c_ex error!\n");
        return HI_FAILURE;
    }
#else
    int ret;

    u8DevNum = os05abusinfo->s8I2cDev;
    ret = snprintf_s(acDevFile, sizeof(acDevFile), sizeof(acDevFile) - 1, "/dev/i2c-%u", u8DevNum);
    if (ret < 0) {
        return HI_FAILURE;
    }
    g_fd[vi_pipe] = open(acDevFile, O_RDWR, S_IRUSR | S_IWUSR);
    if (g_fd[vi_pipe] < 0) {
        SNS_ERR_TRACE("Open /dev/hi_i2c_drv-%u error!\n", u8DevNum);
        return HI_FAILURE;
    }

    ret = ioctl(g_fd[vi_pipe], I2C_SLAVE_FORCE, (OS05A_I2C_ADDR >> 1));
    if (ret < 0) {
        SNS_ERR_TRACE("I2C_SLAVE_FORCE error!\n");
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return ret;
    }
#endif
    return HI_SUCCESS;
}

int os05a_i2c_exit(VI_PIPE vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return HI_SUCCESS;
    }
    return HI_FAILURE;
}

int os05a_read_register(VI_PIPE vi_pipe, HI_U32 addr)
{
    hi_unused(vi_pipe);
    hi_unused(addr);

    return HI_SUCCESS;
}

int os05a_write_register(VI_PIPE vi_pipe, HI_U32 addr, HI_U32 data)
{
    if (g_fd[vi_pipe] < 0) {
        return HI_SUCCESS;
    }

#ifdef HI_GPIO_I2C
    i2c_data.dev_addr = OS05A_I2C_ADDR;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = OS05A_ADDR_BYTE;
    i2c_data.data = data;
    i2c_data.data_byte_num = OS05A_DATA_BYTE;

    ret = ioctl(g_fd[vi_pipe], GPIO_I2C_WRITE, &i2c_data);
    if (ret) {
        SNS_ERR_TRACE("GPIO-I2C write faild!\n");
        return ret;
    }
#else
    HI_U32 idx = 0;
    HI_S32 ret;
    HI_U8 buf[I2C_BUF_NUM];

    if (OS05A_ADDR_BYTE == 2) {  /* 2 byte */
        buf[idx] = (addr >> 8) & 0xff;  /* shift 8 */
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    } else {
    }

    if (OS05A_DATA_BYTE == 2) {  /* 2 byte */
    } else {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[vi_pipe], buf, (OS05A_ADDR_BYTE + OS05A_DATA_BYTE));
    if (ret < 0) {
        SNS_ERR_TRACE("I2C_WRITE error!\n");
        return HI_FAILURE;
    }

#endif
    return HI_SUCCESS;
}

static void delay_ms(int ms)
{
    usleep(ms * 1000);  /* 1ms: 1000us */
    return;
}

void os05a_standby(VI_PIPE vi_pipe)
{
    hi_unused(vi_pipe);
    return;
}

void os05a_restart(VI_PIPE vi_pipe)
{
    hi_unused(vi_pipe);
    return;
}

void os05a_prog(VI_PIPE vi_pipe, const HI_U32 *rom)
{
    int i = 0;

    while (1) {
        HI_U32 lookup = rom[i++];
        HI_U32 addr = (lookup >> 16) & 0xFFFF; /* shift 16 */
        HI_U32 data = lookup & 0xFFFF;
        if (addr == 0xFFFE) {
            delay_ms(data);
        } else if (addr == 0xFFFF) {
            return;
        } else {
            os05a_write_register(vi_pipe, addr, data);
        }
    }
}

void os05a_default_reg_init(VI_PIPE vi_pipe)
{
    HI_U32 i;
    HI_S32 ret = HI_SUCCESS;
    ISP_SNS_STATE_S *pastos05a = HI_NULL;
    pastos05a = os05a_get_ctx(vi_pipe);
    for (i = 0; i < pastos05a->astRegsInfo[0].u32RegNum; i++) {
        ret += os05a_write_register(vi_pipe,
                                    pastos05a->astRegsInfo[0].astI2cData[i].u32RegAddr,
                                    pastos05a->astRegsInfo[0].astI2cData[i].u32Data);
    }
    if (ret != HI_SUCCESS) {
        SNS_ERR_TRACE("write register failed!\n");
    }
    return;
}

void os05a_init(VI_PIPE vi_pipe)
{
    HI_BOOL bInit;
    HI_U8   u8ImgMode;
    HI_S32  ret;
    ISP_SNS_STATE_S *pastos05a = HI_NULL;
    pastos05a = os05a_get_ctx(vi_pipe);
    bInit = pastos05a->bInit;
    u8ImgMode   = pastos05a->u8ImgMode;

    ret = os05a_i2c_init(vi_pipe);
    if (ret != HI_SUCCESS) {
        SNS_ERR_TRACE("i2c init failed!\n");
        return;
    }
    /* When sensor first init, config all registers */
    if (bInit == HI_FALSE) {
        switch (u8ImgMode) {
            case Os05a_5M_2688x1944_12bit_linear30:
                os05a_init_5M_2688x1944_12bit_linear30(vi_pipe);
                break;
            case Os05a_5M_2688x1944_10bit_wdr30:
                os05a_init_5M_2688x1944_10bit_wdr30(vi_pipe);
                break;
            default:
                SNS_ERR_TRACE("Not Support Image Mode %d\n", u8ImgMode);
                break;
        }
    } else {
        /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
        switch (u8ImgMode) {
            case Os05a_5M_2688x1944_12bit_linear30:
                os05a_init_5M_2688x1944_12bit_linear30(vi_pipe);
                break;
            case Os05a_5M_2688x1944_10bit_wdr30:
                os05a_init_5M_2688x1944_10bit_wdr30(vi_pipe);
                break;
            default:
                SNS_ERR_TRACE("Not Support Image Mode %d\n", u8ImgMode);
                break;
        }
    }
    pastos05a->bInit = HI_TRUE;
    return;
}

void os05a_exit(VI_PIPE vi_pipe)
{
    HI_S32 ret;
    ret = os05a_i2c_exit(vi_pipe);
    if (ret != HI_SUCCESS) {
        SNS_ERR_TRACE("i2c exit failed!\n");
    }
    return;
}

void os05a_init_5M_2688x1944_10bit_wdr30(VI_PIPE vi_pipe)
{
    HI_S32 ret = HI_SUCCESS;

    ret += os05a_write_register(vi_pipe, 0x4600, 0x01);
    ret += os05a_write_register(vi_pipe, 0x4601, 0x04);
    ret += os05a_write_register(vi_pipe, 0x4603, 0x00);
    ret += os05a_write_register(vi_pipe, 0x0103, 0x01);
    ret += os05a_write_register(vi_pipe, 0x0303, 0x01);
    ret += os05a_write_register(vi_pipe, 0x0305, 0x5a);
    ret += os05a_write_register(vi_pipe, 0x0306, 0x00);
    ret += os05a_write_register(vi_pipe, 0x0307, 0x00);
    ret += os05a_write_register(vi_pipe, 0x0308, 0x03);
    ret += os05a_write_register(vi_pipe, 0x0309, 0x04);
    ret += os05a_write_register(vi_pipe, 0x032a, 0x00);
    ret += os05a_write_register(vi_pipe, 0x031e, 0x09);
    ret += os05a_write_register(vi_pipe, 0x0325, 0x48);
    ret += os05a_write_register(vi_pipe, 0x0328, 0x07);
    ret += os05a_write_register(vi_pipe, 0x300d, 0x11);
    ret += os05a_write_register(vi_pipe, 0x300e, 0x11);
    ret += os05a_write_register(vi_pipe, 0x300f, 0x11);
    ret += os05a_write_register(vi_pipe, 0x3026, 0x00);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3027, 0x00);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3010, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3012, 0x41);
    ret += os05a_write_register(vi_pipe, 0x3016, 0xf0);
    ret += os05a_write_register(vi_pipe, 0x3018, 0xf0);
    ret += os05a_write_register(vi_pipe, 0x3028, 0xf0);
    ret += os05a_write_register(vi_pipe, 0x301e, 0x98);
    ret += os05a_write_register(vi_pipe, 0x3010, 0x01);  /* update 0x04 */
    ret += os05a_write_register(vi_pipe, 0x3011, 0x04);  /* update 0x06 */
    ret += os05a_write_register(vi_pipe, 0x3031, 0xa9);
    ret += os05a_write_register(vi_pipe, 0x3103, 0x48);
    ret += os05a_write_register(vi_pipe, 0x3104, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3106, 0x10);
    ret += os05a_write_register(vi_pipe, 0x3501, 0x09);
    ret += os05a_write_register(vi_pipe, 0x3502, 0xa0);
    ret += os05a_write_register(vi_pipe, 0x3505, 0x83);
    ret += os05a_write_register(vi_pipe, 0x3508, 0x03);
    ret += os05a_write_register(vi_pipe, 0x3509, 0x00);
    ret += os05a_write_register(vi_pipe, 0x350a, 0x04);
    ret += os05a_write_register(vi_pipe, 0x350b, 0x00);
    ret += os05a_write_register(vi_pipe, 0x350c, 0x03);
    ret += os05a_write_register(vi_pipe, 0x350d, 0x00);
    ret += os05a_write_register(vi_pipe, 0x350e, 0x04);
    ret += os05a_write_register(vi_pipe, 0x350f, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3600, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3626, 0xff);
    ret += os05a_write_register(vi_pipe, 0x3605, 0x50);
    ret += os05a_write_register(vi_pipe, 0x3609, 0xb5);
    ret += os05a_write_register(vi_pipe, 0x3610, 0x69);
    ret += os05a_write_register(vi_pipe, 0x360c, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3628, 0xa4);
    ret += os05a_write_register(vi_pipe, 0x3629, 0x6a);
    ret += os05a_write_register(vi_pipe, 0x362d, 0x10);
    ret += os05a_write_register(vi_pipe, 0x36c0, 0x00);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3660, 0x42);
    ret += os05a_write_register(vi_pipe, 0x3661, 0x07);
    ret += os05a_write_register(vi_pipe, 0x3662, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3663, 0x28);
    ret += os05a_write_register(vi_pipe, 0x3664, 0x0d);
    ret += os05a_write_register(vi_pipe, 0x366a, 0x38);
    ret += os05a_write_register(vi_pipe, 0x366b, 0xa0);
    ret += os05a_write_register(vi_pipe, 0x366d, 0x00);
    ret += os05a_write_register(vi_pipe, 0x366e, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3680, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3621, 0x81);
    ret += os05a_write_register(vi_pipe, 0x3634, 0x31);
    ret += os05a_write_register(vi_pipe, 0x3620, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3622, 0x00);
    ret += os05a_write_register(vi_pipe, 0x362a, 0xd0);
    ret += os05a_write_register(vi_pipe, 0x362e, 0x8c);
    ret += os05a_write_register(vi_pipe, 0x362f, 0x98);
    ret += os05a_write_register(vi_pipe, 0x3630, 0xb0);
    ret += os05a_write_register(vi_pipe, 0x3631, 0xd7);
    ret += os05a_write_register(vi_pipe, 0x3701, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x3737, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3740, 0x18);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3741, 0x04);
    ret += os05a_write_register(vi_pipe, 0x373c, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x373b, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3705, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3706, 0x50);
    ret += os05a_write_register(vi_pipe, 0x370a, 0x00);
    ret += os05a_write_register(vi_pipe, 0x370b, 0xe4);
    ret += os05a_write_register(vi_pipe, 0x3709, 0x4a);
    ret += os05a_write_register(vi_pipe, 0x3714, 0x21);
    ret += os05a_write_register(vi_pipe, 0x371c, 0x00);
    ret += os05a_write_register(vi_pipe, 0x371d, 0x08);
    ret += os05a_write_register(vi_pipe, 0x375e, 0x0e);  /* update 0x0b */
    ret += os05a_write_register(vi_pipe, 0x3760, 0x13);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3776, 0x10);
    ret += os05a_write_register(vi_pipe, 0x3781, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3782, 0x04);
    ret += os05a_write_register(vi_pipe, 0x3783, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3784, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3785, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3788, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3789, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3797, 0x04);
    ret += os05a_write_register(vi_pipe, 0x3798, 0x01);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3799, 0x00);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3761, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3762, 0x0d);
    ret += os05a_write_register(vi_pipe, 0x3800, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3801, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3802, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3803, 0x0c);
    ret += os05a_write_register(vi_pipe, 0x3804, 0x0e);
    ret += os05a_write_register(vi_pipe, 0x3805, 0xff);
    ret += os05a_write_register(vi_pipe, 0x3806, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3807, 0x6f);
    ret += os05a_write_register(vi_pipe, 0x3808, 0x0a);
    ret += os05a_write_register(vi_pipe, 0x3809, 0x80);
    ret += os05a_write_register(vi_pipe, 0x380a, 0x07);
    ret += os05a_write_register(vi_pipe, 0x380b, 0x98);
    ret += os05a_write_register(vi_pipe, 0x380c, 0x02);
    ret += os05a_write_register(vi_pipe, 0x380d, 0xd0);
    ret += os05a_write_register(vi_pipe, 0x380e, 0x09);
    ret += os05a_write_register(vi_pipe, 0x380f, 0xc0);
    ret += os05a_write_register(vi_pipe, 0x3811, 0x10);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3813, 0x04);
    ret += os05a_write_register(vi_pipe, 0x3814, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3815, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3816, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3817, 0x01);
    ret += os05a_write_register(vi_pipe, 0x381c, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3820, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3821, 0x24);
    ret += os05a_write_register(vi_pipe, 0x3823, 0x08);  /* update 0x18 */
    ret += os05a_write_register(vi_pipe, 0x3826, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3827, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3833, 0x01);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3832, 0x02);
    ret += os05a_write_register(vi_pipe, 0x383c, 0x48);
    ret += os05a_write_register(vi_pipe, 0x383d, 0xff);
    ret += os05a_write_register(vi_pipe, 0x3843, 0x20);
    ret += os05a_write_register(vi_pipe, 0x382d, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3d85, 0x0b);
    ret += os05a_write_register(vi_pipe, 0x3d84, 0x40);
    ret += os05a_write_register(vi_pipe, 0x3d8c, 0x63);
    ret += os05a_write_register(vi_pipe, 0x3d8d, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4000, 0x78);
    ret += os05a_write_register(vi_pipe, 0x4001, 0x2b);
    ret += os05a_write_register(vi_pipe, 0x4004, 0x00);  /* update */
    ret += os05a_write_register(vi_pipe, 0x4005, 0x40);
    ret += os05a_write_register(vi_pipe, 0x4028, 0x2f);
    ret += os05a_write_register(vi_pipe, 0x400a, 0x01);
    ret += os05a_write_register(vi_pipe, 0x4010, 0x12);
    ret += os05a_write_register(vi_pipe, 0x4008, 0x02);
    ret += os05a_write_register(vi_pipe, 0x4009, 0x0d);
    ret += os05a_write_register(vi_pipe, 0x401a, 0x58);
    ret += os05a_write_register(vi_pipe, 0x4050, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4051, 0x01);
    ret += os05a_write_register(vi_pipe, 0x4052, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4053, 0x80);
    ret += os05a_write_register(vi_pipe, 0x4054, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4055, 0x80);
    ret += os05a_write_register(vi_pipe, 0x4056, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4057, 0x80);
    ret += os05a_write_register(vi_pipe, 0x4058, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4059, 0x80);
    ret += os05a_write_register(vi_pipe, 0x430b, 0xff);
    ret += os05a_write_register(vi_pipe, 0x430c, 0xff);
    ret += os05a_write_register(vi_pipe, 0x430d, 0x00);
    ret += os05a_write_register(vi_pipe, 0x430e, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4501, 0x18);
    ret += os05a_write_register(vi_pipe, 0x4502, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4643, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4640, 0x01);
    ret += os05a_write_register(vi_pipe, 0x4641, 0x04);
    ret += os05a_write_register(vi_pipe, 0x480e, 0x04);
    ret += os05a_write_register(vi_pipe, 0x4813, 0x98);
    ret += os05a_write_register(vi_pipe, 0x4815, 0x2b);
    ret += os05a_write_register(vi_pipe, 0x486e, 0x36);
    ret += os05a_write_register(vi_pipe, 0x486f, 0x84);
    ret += os05a_write_register(vi_pipe, 0x4860, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4861, 0xa0);
    ret += os05a_write_register(vi_pipe, 0x484b, 0x05);
    ret += os05a_write_register(vi_pipe, 0x4850, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4851, 0xaa);
    ret += os05a_write_register(vi_pipe, 0x4852, 0xff);
    ret += os05a_write_register(vi_pipe, 0x4853, 0x8a);
    ret += os05a_write_register(vi_pipe, 0x4854, 0x08);
    ret += os05a_write_register(vi_pipe, 0x4855, 0x30);
    ret += os05a_write_register(vi_pipe, 0x4800, 0x60);  /* update 0x00 */
    ret += os05a_write_register(vi_pipe, 0x4837, 0x0b);
    ret += os05a_write_register(vi_pipe, 0x484a, 0x3f);  /* update */
    ret += os05a_write_register(vi_pipe, 0x5000, 0xc9);
    ret += os05a_write_register(vi_pipe, 0x5001, 0x43);
    ret += os05a_write_register(vi_pipe, 0x5002, 0x00);  /* update */
    ret += os05a_write_register(vi_pipe, 0x5211, 0x03);
    ret += os05a_write_register(vi_pipe, 0x5291, 0x03);
    ret += os05a_write_register(vi_pipe, 0x520d, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x520e, 0xfd);
    ret += os05a_write_register(vi_pipe, 0x520f, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x5210, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x528d, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x528e, 0xfd);
    ret += os05a_write_register(vi_pipe, 0x528f, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x5290, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x5004, 0x40);
    ret += os05a_write_register(vi_pipe, 0x5005, 0x00);
    ret += os05a_write_register(vi_pipe, 0x5180, 0x00);
    ret += os05a_write_register(vi_pipe, 0x5181, 0x10);
    ret += os05a_write_register(vi_pipe, 0x5182, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x5183, 0xff);
    ret += os05a_write_register(vi_pipe, 0x580b, 0x03);

    ret += os05a_write_register(vi_pipe, 0x4d00, 0x03);
    ret += os05a_write_register(vi_pipe, 0x4d01, 0xe9);
    ret += os05a_write_register(vi_pipe, 0x4d02, 0xba);
    ret += os05a_write_register(vi_pipe, 0x4d03, 0x66);
    ret += os05a_write_register(vi_pipe, 0x4d04, 0x46);
    ret += os05a_write_register(vi_pipe, 0x4d05, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x3603, 0x3c);
    ret += os05a_write_register(vi_pipe, 0x3703, 0x26);
    ret += os05a_write_register(vi_pipe, 0x3709, 0x49);

    ret += os05a_write_register(vi_pipe, 0x3708, 0x2d);  /* update */
    ret += os05a_write_register(vi_pipe, 0x3719, 0x1c);  /* update */
    ret += os05a_write_register(vi_pipe, 0x371a, 0x06);  /* update */

    ret += os05a_write_register(vi_pipe, 0x4000, 0x79);
    ret += os05a_write_register(vi_pipe, 0x380c, 0x02);
    ret += os05a_write_register(vi_pipe, 0x380d, 0xd0);
    ret += os05a_write_register(vi_pipe, 0x380e, 0x09);
    ret += os05a_write_register(vi_pipe, 0x380f, 0xc0);
    ret += os05a_write_register(vi_pipe, 0x3501, 0x05);
    ret += os05a_write_register(vi_pipe, 0x3502, 0xdd);
    ret += os05a_write_register(vi_pipe, 0x3511, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3512, 0x77);

    os05a_default_reg_init(vi_pipe);
    ret += os05a_write_register(vi_pipe, 0x0100, 0x01);

    ret += os05a_write_register(vi_pipe, 0x0100, 0x00);

    ret += os05a_write_register(vi_pipe, 0x0100, 0x01);
    if (ret != HI_SUCCESS) {
        SNS_ERR_TRACE("write register failed!\n");
        return;
    }
    printf("-------OV os05a_init_5M_2688x1944_10bit_wdr30 Initial OK!-------\n");
    return;
}

void os05a_init_5M_2688x1944_12bit_linear30(VI_PIPE vi_pipe)
{
    HI_S32 ret = HI_SUCCESS;

    ret += os05a_write_register(vi_pipe, 0x4600, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4601, 0x10);
    ret += os05a_write_register(vi_pipe, 0x4603, 0x01);

    ret += os05a_write_register(vi_pipe, 0x0103, 0x01);
    ret += os05a_write_register(vi_pipe, 0x0303, 0x01);
    ret += os05a_write_register(vi_pipe, 0x0305, 0x27);
    ret += os05a_write_register(vi_pipe, 0x0306, 0x00);
    ret += os05a_write_register(vi_pipe, 0x0307, 0x00);
    ret += os05a_write_register(vi_pipe, 0x0308, 0x03);
    ret += os05a_write_register(vi_pipe, 0x0309, 0x04);
    ret += os05a_write_register(vi_pipe, 0x032a, 0x00);
    ret += os05a_write_register(vi_pipe, 0x031e, 0x0a);
    ret += os05a_write_register(vi_pipe, 0x0325, 0x48);
    ret += os05a_write_register(vi_pipe, 0x0328, 0x07);
    ret += os05a_write_register(vi_pipe, 0x300d, 0x11);
    ret += os05a_write_register(vi_pipe, 0x300e, 0x11);
    ret += os05a_write_register(vi_pipe, 0x300f, 0x11);
    ret += os05a_write_register(vi_pipe, 0x3010, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3012, 0x41);
    ret += os05a_write_register(vi_pipe, 0x3016, 0xf0);
    ret += os05a_write_register(vi_pipe, 0x3018, 0xf0);
    ret += os05a_write_register(vi_pipe, 0x3028, 0xf0);
    ret += os05a_write_register(vi_pipe, 0x301e, 0x98);
    ret += os05a_write_register(vi_pipe, 0x3010, 0x04);
    ret += os05a_write_register(vi_pipe, 0x3011, 0x06);
    ret += os05a_write_register(vi_pipe, 0x3031, 0xa9);
    ret += os05a_write_register(vi_pipe, 0x3103, 0x48);
    ret += os05a_write_register(vi_pipe, 0x3104, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3106, 0x10);
    ret += os05a_write_register(vi_pipe, 0x3400, 0x04);
    ret += os05a_write_register(vi_pipe, 0x3025, 0x03);
    ret += os05a_write_register(vi_pipe, 0x3425, 0x51);
    ret += os05a_write_register(vi_pipe, 0x3428, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3406, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3408, 0x03);
    ret += os05a_write_register(vi_pipe, 0x3501, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3502, 0x6f);
    ret += os05a_write_register(vi_pipe, 0x3505, 0x83);
    ret += os05a_write_register(vi_pipe, 0x3508, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3509, 0x80);
    ret += os05a_write_register(vi_pipe, 0x350a, 0x04);
    ret += os05a_write_register(vi_pipe, 0x350b, 0x00);
    ret += os05a_write_register(vi_pipe, 0x350c, 0x00);
    ret += os05a_write_register(vi_pipe, 0x350d, 0x80);
    ret += os05a_write_register(vi_pipe, 0x350e, 0x04);
    ret += os05a_write_register(vi_pipe, 0x350f, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3600, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3626, 0xff);
    ret += os05a_write_register(vi_pipe, 0x3605, 0x50);
    ret += os05a_write_register(vi_pipe, 0x3609, 0xdb);
    ret += os05a_write_register(vi_pipe, 0x3610, 0x69);
    ret += os05a_write_register(vi_pipe, 0x360c, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3628, 0xa4);
    ret += os05a_write_register(vi_pipe, 0x3629, 0x6a);
    ret += os05a_write_register(vi_pipe, 0x362d, 0x10);
    ret += os05a_write_register(vi_pipe, 0x3660, 0xd3);
    ret += os05a_write_register(vi_pipe, 0x3661, 0x06);
    ret += os05a_write_register(vi_pipe, 0x3662, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3663, 0x28);
    ret += os05a_write_register(vi_pipe, 0x3664, 0x0d);
    ret += os05a_write_register(vi_pipe, 0x366a, 0x38);
    ret += os05a_write_register(vi_pipe, 0x366b, 0xa0);
    ret += os05a_write_register(vi_pipe, 0x366d, 0x00);
    ret += os05a_write_register(vi_pipe, 0x366e, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3680, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3621, 0x81);
    ret += os05a_write_register(vi_pipe, 0x3634, 0x31);
    ret += os05a_write_register(vi_pipe, 0x3620, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3622, 0x00);
    ret += os05a_write_register(vi_pipe, 0x362a, 0xd0);
    ret += os05a_write_register(vi_pipe, 0x362e, 0x8c);
    ret += os05a_write_register(vi_pipe, 0x362f, 0x98);
    ret += os05a_write_register(vi_pipe, 0x3630, 0xb0);
    ret += os05a_write_register(vi_pipe, 0x3631, 0xd7);
    ret += os05a_write_register(vi_pipe, 0x3701, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x3737, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3741, 0x04);
    ret += os05a_write_register(vi_pipe, 0x373c, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x373b, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3705, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3706, 0xa0);
    ret += os05a_write_register(vi_pipe, 0x370a, 0x01);
    ret += os05a_write_register(vi_pipe, 0x370b, 0xc8);
    ret += os05a_write_register(vi_pipe, 0x3709, 0x4a);
    ret += os05a_write_register(vi_pipe, 0x3714, 0x21);
    ret += os05a_write_register(vi_pipe, 0x371c, 0x00);
    ret += os05a_write_register(vi_pipe, 0x371d, 0x08);
    ret += os05a_write_register(vi_pipe, 0x375e, 0x0b);
    ret += os05a_write_register(vi_pipe, 0x3776, 0x10);
    ret += os05a_write_register(vi_pipe, 0x3781, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3782, 0x04);
    ret += os05a_write_register(vi_pipe, 0x3783, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3784, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3785, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3788, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3789, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3797, 0x04);
    ret += os05a_write_register(vi_pipe, 0x3761, 0x02);
    ret += os05a_write_register(vi_pipe, 0x3762, 0x0d);
    ret += os05a_write_register(vi_pipe, 0x3800, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3801, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3802, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3803, 0x0c);
    ret += os05a_write_register(vi_pipe, 0x3804, 0x0e);
    ret += os05a_write_register(vi_pipe, 0x3805, 0xff);
    ret += os05a_write_register(vi_pipe, 0x3806, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3807, 0x6f);
    ret += os05a_write_register(vi_pipe, 0x3808, 0x0a);
    ret += os05a_write_register(vi_pipe, 0x3809, 0x80);
    ret += os05a_write_register(vi_pipe, 0x380a, 0x07);
    ret += os05a_write_register(vi_pipe, 0x380b, 0x98);
    ret += os05a_write_register(vi_pipe, 0x380c, 0x04);
    ret += os05a_write_register(vi_pipe, 0x380d, 0xd0);
    ret += os05a_write_register(vi_pipe, 0x380e, 0x08);
    ret += os05a_write_register(vi_pipe, 0x380f, 0x8f);
    ret += os05a_write_register(vi_pipe, 0x3813, 0x04);
    ret += os05a_write_register(vi_pipe, 0x3814, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3815, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3816, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3817, 0x01);
    ret += os05a_write_register(vi_pipe, 0x381c, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3820, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3821, 0x04);
    ret += os05a_write_register(vi_pipe, 0x3823, 0x18);
    ret += os05a_write_register(vi_pipe, 0x3826, 0x00);
    ret += os05a_write_register(vi_pipe, 0x3827, 0x01);
    ret += os05a_write_register(vi_pipe, 0x3832, 0x02);
    ret += os05a_write_register(vi_pipe, 0x383c, 0x48);
    ret += os05a_write_register(vi_pipe, 0x383d, 0xff);
    ret += os05a_write_register(vi_pipe, 0x3843, 0x20);

    ret += os05a_write_register(vi_pipe, 0x382d, 0x08);
    ret += os05a_write_register(vi_pipe, 0x3d85, 0x0b);
    ret += os05a_write_register(vi_pipe, 0x3d84, 0x40);
    ret += os05a_write_register(vi_pipe, 0x3d8c, 0x63);
    ret += os05a_write_register(vi_pipe, 0x3d8d, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4000, 0x78);
    ret += os05a_write_register(vi_pipe, 0x4001, 0x2b);
    ret += os05a_write_register(vi_pipe, 0x4005, 0x40);
    ret += os05a_write_register(vi_pipe, 0x4028, 0x2f);
    ret += os05a_write_register(vi_pipe, 0x400a, 0x01);
    ret += os05a_write_register(vi_pipe, 0x4010, 0x12);
    ret += os05a_write_register(vi_pipe, 0x4008, 0x02);
    ret += os05a_write_register(vi_pipe, 0x4009, 0x0d);
    ret += os05a_write_register(vi_pipe, 0x401a, 0x58);
    ret += os05a_write_register(vi_pipe, 0x4050, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4051, 0x01);
    ret += os05a_write_register(vi_pipe, 0x4052, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4053, 0x80);
    ret += os05a_write_register(vi_pipe, 0x4054, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4055, 0x80);
    ret += os05a_write_register(vi_pipe, 0x4056, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4057, 0x80);
    ret += os05a_write_register(vi_pipe, 0x4058, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4059, 0x80);
    ret += os05a_write_register(vi_pipe, 0x430b, 0xff);
    ret += os05a_write_register(vi_pipe, 0x430c, 0xff);
    ret += os05a_write_register(vi_pipe, 0x430d, 0x00);
    ret += os05a_write_register(vi_pipe, 0x430e, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4501, 0x18);
    ret += os05a_write_register(vi_pipe, 0x4502, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4600, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4601, 0x10);
    ret += os05a_write_register(vi_pipe, 0x4603, 0x01);
    ret += os05a_write_register(vi_pipe, 0x4643, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4640, 0x01);
    ret += os05a_write_register(vi_pipe, 0x4641, 0x04);
    ret += os05a_write_register(vi_pipe, 0x480e, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4813, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4815, 0x2b);
    ret += os05a_write_register(vi_pipe, 0x486e, 0x36);
    ret += os05a_write_register(vi_pipe, 0x486f, 0x84);
    ret += os05a_write_register(vi_pipe, 0x4860, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4861, 0xa0);
    ret += os05a_write_register(vi_pipe, 0x484b, 0x05);
    ret += os05a_write_register(vi_pipe, 0x4850, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4851, 0xaa);
    ret += os05a_write_register(vi_pipe, 0x4852, 0xff);
    ret += os05a_write_register(vi_pipe, 0x4853, 0x8a);
    ret += os05a_write_register(vi_pipe, 0x4854, 0x08);
    ret += os05a_write_register(vi_pipe, 0x4855, 0x30);
    ret += os05a_write_register(vi_pipe, 0x4800, 0x00);
    ret += os05a_write_register(vi_pipe, 0x4837, 0x19);
    ret += os05a_write_register(vi_pipe, 0x5000, 0xc9);
    ret += os05a_write_register(vi_pipe, 0x5001, 0x43);
    ret += os05a_write_register(vi_pipe, 0x5211, 0x03);
    ret += os05a_write_register(vi_pipe, 0x5291, 0x03);
    ret += os05a_write_register(vi_pipe, 0x520d, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x520e, 0xfd);
    ret += os05a_write_register(vi_pipe, 0x520f, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x5210, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x528d, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x528e, 0xfd);
    ret += os05a_write_register(vi_pipe, 0x528f, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x5290, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x5004, 0x40);
    ret += os05a_write_register(vi_pipe, 0x5005, 0x00);
    ret += os05a_write_register(vi_pipe, 0x5180, 0x00);
    ret += os05a_write_register(vi_pipe, 0x5181, 0x10);
    ret += os05a_write_register(vi_pipe, 0x5182, 0x0f);
    ret += os05a_write_register(vi_pipe, 0x5183, 0xff);
    ret += os05a_write_register(vi_pipe, 0x580b, 0x03);

    ret += os05a_write_register(vi_pipe, 0x4d00, 0x03);
    ret += os05a_write_register(vi_pipe, 0x4d01, 0xe9);
    ret += os05a_write_register(vi_pipe, 0x4d02, 0xba);
    ret += os05a_write_register(vi_pipe, 0x4d03, 0x66);
    ret += os05a_write_register(vi_pipe, 0x4d04, 0x46);
    ret += os05a_write_register(vi_pipe, 0x4d05, 0xa5);
    ret += os05a_write_register(vi_pipe, 0x3603, 0x3c);
    ret += os05a_write_register(vi_pipe, 0x3703, 0x26);
    ret += os05a_write_register(vi_pipe, 0x3709, 0x49);
    ret += os05a_write_register(vi_pipe, 0x4000, 0x79);
    ret += os05a_write_register(vi_pipe, 0x380c, 0x06);
    ret += os05a_write_register(vi_pipe, 0x380d, 0x04);
    ret += os05a_write_register(vi_pipe, 0x380e, 0x09);
    ret += os05a_write_register(vi_pipe, 0x380f, 0x21);
    ret += os05a_write_register(vi_pipe, 0x3501, 0x09);
    ret += os05a_write_register(vi_pipe, 0x3502, 0x19);

    os05a_default_reg_init(vi_pipe);

    ret += os05a_write_register(vi_pipe, 0x0100, 0x01);

    ret += os05a_write_register(vi_pipe, 0x0100, 0x00);

    ret += os05a_write_register(vi_pipe, 0x0100, 0x01);
    if (ret != HI_SUCCESS) {
        SNS_ERR_TRACE("write register failed!\n");
        return;
    }
    printf("-------OV os05a_init_5M_2688x1944_12bit_linear30 Initial OK!-------\n");
    return;
}

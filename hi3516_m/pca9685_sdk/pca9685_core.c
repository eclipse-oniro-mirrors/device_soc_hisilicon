/*
 * pca9685_core.c - PCA9685 驱动核心层(用户态 SDK 版)
 * 移植自内核版 mytest/pca9685/pca9685_core.c:
 *   - I2C 收发 API 同名同参:内核走 khdf i2c_if,这里走 uhdf i2c_if_u
 *     (HdfIoServiceBind -> HDF_PLATFORM_I2C_MANAGER, /dev/hdf/dev_mgr)
 *   - LOS_TaskDelay -> PcaDelayMs(nanosleep),PRINTK -> printf(pca_port.h)
 *   - 其余逻辑/寄存器时序与内核版逐行一致
 * 协议与 Arduino Adafruit_PWMServoDriver 库一致:
 *   begin()        -> PcaInitChip 里的复位 + MODE2 配置
 *   setPWMFreq(50) -> SLEEP + PRESCALE=0x79 + 唤醒
 *   setPWM(ch,0,n) -> PcaWriteCh 写 ON/OFF 计数
 */
#include "pca9685.h"
#include "pca_port.h"
#include "i2c_if.h"         /* I2cOpen / I2cTransfer / I2cClose(用户态 HDI) */

/* PCA9685 寄存器(手册 Table 2) */
#define PCA_MODE1      0x00
#define PCA_MODE2      0x01
#define PCA_LED0_ON_L  0x06     /* 每通道 4 字节:ON_L ON_H OFF_L OFF_H,AI=1 地址自动递增 */
#define PCA_ALL_LED_L  0xFA     /* 全通道 ON/OFF,写一次 4 字节生效到 16 路 */
#define PCA_PRESCALE   0xFE
#define MODE1_SLEEP    0x10
#define MODE1_AI       0x20     /* 地址自动递增 */
#define MODE1_RESTART  0x80
#define MODE2_OUTDRV   0x04     /* 推挽输出,驱动舵机用 */

#define SERVO_FREQ     50       /* 舵机 50Hz,周期 20ms */
#define PRESCALE_50HZ  0x79     /* round(25MHz/(4096*50))-1 = 121 */

#define BUS_WAIT_RETRY 100      /* 等 HDF 注册 I2C 控制器:100 次 x 10ms = 1s */
#define SCAN_ADDR_MIN  0x03
#define SCAN_ADDR_MAX  0x77

static DevHandle g_bus;
static uint8_t   g_busNum = PCA_BUS_DEFAULT;
static uint8_t   g_addr   = PCA_ADDR_DEFAULT;
static int32_t   g_ready;               /* 1 = 芯片已初始化(本进程内) */

/* 写单寄存器 */
static int32_t PcaWriteReg(uint8_t reg, uint8_t val)
{
    uint8_t buf[2] = { reg, val };
    struct I2cMsg msg = { .addr = g_addr, .buf = buf, .len = 2, .flags = 0 };
    return (I2cTransfer(g_bus, &msg, 1) == 1) ? 0 : -1;
}

/* 读单寄存器(写寄存器号 + 读 1 字节) */
static int32_t PcaReadReg(uint8_t reg, uint8_t *val)
{
    struct I2cMsg msgs[2] = {
        { .addr = g_addr, .buf = &reg, .len = 1, .flags = 0 },
        { .addr = g_addr, .buf = val,  .len = 1, .flags = I2C_FLAG_READ },
    };
    return (I2cTransfer(g_bus, msgs, 2) == 2) ? 0 : -1;
}

/* 写某通道 ON/OFF(12 位计数,0~4095) */
static int32_t PcaWriteCh(uint8_t ch, uint16_t on, uint16_t off)
{
    uint8_t buf[5] = { (uint8_t)(PCA_LED0_ON_L + 4 * ch),
                       (uint8_t)(on & 0xFF), (uint8_t)(on >> 8),
                       (uint8_t)(off & 0xFF), (uint8_t)(off >> 8) };
    struct I2cMsg msg = { .addr = g_addr, .buf = buf, .len = 5, .flags = 0 };
    return (I2cTransfer(g_bus, &msg, 1) == 1) ? 0 : -1;
}

/* 打开总线,HDF 未注册好时等一会(业务一般在开机后较久才跑,通常一次就成) */
static int32_t PcaOpenBusWait(uint8_t bus)
{
    int32_t retry;

    for (retry = 0; retry < BUS_WAIT_RETRY; retry++) {
        g_bus = I2cOpen(bus);
        if (g_bus != NULL) {
            g_busNum = bus;
            return HDF_SUCCESS;
        }
        PcaDelayMs(10);
    }
    PRINTK("pca9685: open i2c bus %u fail after %d tries\n", bus, BUS_WAIT_RETRY);
    return HDF_FAILURE;
}

/* 对应 Arduino 的 begin() + setPWMFreq(50);写 MODE1 有 ACK 说明器件在线 */
static int32_t PcaInitChip(void)
{
    uint8_t mode1 = 0;

    if (PcaWriteReg(PCA_MODE1, 0x00) != 0) {
        PRINTK("pca9685: no ACK @0x%02x on bus %u (查地址/接线/供电)\n", g_addr, g_busNum);
        return HDF_FAILURE;
    }
    (void)PcaWriteReg(PCA_MODE2, MODE2_OUTDRV);            /* 推挽输出 */
    (void)PcaWriteReg(PCA_MODE1, MODE1_SLEEP);             /* 进 SLEEP 才允许改分频 */
    (void)PcaWriteReg(PCA_PRESCALE, PRESCALE_50HZ);        /* 50Hz */
    (void)PcaWriteReg(PCA_MODE1, MODE1_RESTART | MODE1_AI); /* 唤醒 + 地址自动递增 */
    PcaDelayMs(1);                                         /* 唤醒稳定(库里的 delay) */

    if (PcaReadReg(PCA_MODE1, &mode1) != 0) {              /* 回读验证链路 */
        PRINTK("pca9685: read MODE1 fail\n");
        return HDF_FAILURE;
    }
    PRINTK("pca9685: init ok, bus %u @0x%02x, %uHz, MODE1=0x%02x(expect 0x%02x)\n",
           g_busNum, g_addr, SERVO_FREQ, mode1, MODE1_AI);
    return HDF_SUCCESS;
}

int32_t PcaEnsureReady(uint8_t bus, uint8_t addr, int32_t force)
{
    if ((g_ready == 1) && (force == 0) &&
        (bus == g_busNum) && (addr == g_addr)) {
        return HDF_SUCCESS;
    }
    if (g_bus != NULL) {                 /* 换总线/地址时先关旧句柄 */
        I2cClose(g_bus);
        g_bus = NULL;
    }
    g_ready  = 0;
    g_busNum = bus;
    g_addr   = addr;
    if (PcaOpenBusWait(bus) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    if (PcaInitChip() != HDF_SUCCESS) {
        return HDF_FAILURE;
    }
    g_ready = 1;
    (void)PcaAllChannelsOff();           /* 初始化后先全通道无脉冲,舵机松弛不乱动 */
    return HDF_SUCCESS;
}

/* 一键初始化(业务代码入口):默认总线/地址,幂等,可重复调用 */
int32_t PcaInit(void)
{
    return PcaInitEx(PCA_BUS_DEFAULT, PCA_ADDR_DEFAULT);
}

int32_t PcaInitEx(uint8_t bus, uint8_t addr)
{
    return PcaEnsureReady(bus, addr, 0);
}

int32_t PcaScanBus(uint8_t bus)
{
    DevHandle busH = NULL;
    uint8_t regIdx = 0x00;               /* 只发寄存器号不写数据,对 PCA9685 无副作用 */
    int32_t addr;
    int32_t found = 0;
    int32_t retry;

    for (retry = 0; retry < BUS_WAIT_RETRY; retry++) {
        busH = I2cOpen(bus);
        if (busH != NULL) {
            break;
        }
        PcaDelayMs(10);
    }
    if (busH == NULL) {
        PRINTK("pca9685: open i2c bus %u fail, scan abort\n", bus);
        return -1;
    }
    PRINTK("pca9685: scanning bus %u (0x%02x~0x%02x)...\n", bus, SCAN_ADDR_MIN, SCAN_ADDR_MAX);
    for (addr = SCAN_ADDR_MIN; addr <= SCAN_ADDR_MAX; addr++) {
        struct I2cMsg msg = { .addr = (uint16_t)addr, .buf = &regIdx, .len = 1, .flags = 0 };
        if (I2cTransfer(busH, &msg, 1) == 1) {
            PRINTK("  addr 0x%02x ACK%s\n", addr,
                   (addr == PCA_ADDR_DEFAULT) ? "  <- PCA9685 默认地址" : "");
            found++;
        }
    }
    I2cClose(busH);
    if (found == 0) {
        PRINTK("pca9685: scan done, no device (查:VCC->3.3V,SDA/SCL->板上 I2C6 接口,共地)\n");
    } else {
        PRINTK("pca9685: scan done, %d device(s)\n", found);
    }
    return found;
}

int32_t PcaServoUs(uint8_t ch, uint32_t us)
{
    uint16_t off;

    if ((g_ready != 1) || (ch > PCA_SERVO_CH_MAX)) {
        PRINTK("pca9685: not ready or bad ch %u, run 'pca_demo init' first\n", ch);
        return HDF_FAILURE;
    }
    if (us < 500)  { us = 500; }
    if (us > 2500) { us = 2500; }
    off = (uint16_t)((uint32_t)us * 4096 / 20000); /* 500->102, 1500->307, 2500->512 */
    return PcaWriteCh(ch, 0, off);
}

int32_t PcaServoAngle(uint8_t ch, uint32_t deg)
{
    uint32_t us;

    if (deg > 180) { deg = 180; }
    us = 500 + deg * 2000 / 180;         /* 角度 -> 脉宽 */
    return PcaServoUs(ch, us);
}

int32_t PcaAllChannelsOff(void)
{
    uint8_t buf[5] = { PCA_ALL_LED_L, 0x00, 0x00, 0x00, 0x00 };
    struct I2cMsg msg = { .addr = g_addr, .buf = buf, .len = 5, .flags = 0 };

    if (g_bus == NULL) {
        return HDF_FAILURE;
    }
    return (I2cTransfer(g_bus, &msg, 1) == 1) ? 0 : -1;
}

void PcaGetStatus(uint8_t *bus, uint8_t *addr, int32_t *ready)
{
    if (bus != NULL)   { *bus = g_busNum; }
    if (addr != NULL)  { *addr = g_addr; }
    if (ready != NULL) { *ready = g_ready; }
}

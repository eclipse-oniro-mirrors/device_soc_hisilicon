/*
 * pca9685_sdk - PCA9685(16 路 PWM, I2C)舵机驱动 用户态 SDK
 * 与内核版(device/soc/.../mytest/pca9685/pca9685.h)同名同 API:
 *   差别仅一处 —— 运行态在用户进程里,I2C 走 HDF 用户态 HDI
 *   (i2c_if_u: HdfIoServiceBind -> HDF_PLATFORM_I2C_MANAGER),
 *   状态(g_ready 等)随进程各自独立,每个进程先调一次 PcaInit。
 *
 * 业务代码用法(与内核版一字不差):
 *   #include "pca9685.h"
 *   #include "otto_pca.h"
 *
 *   PcaInit();                     一键初始化(幂等)
 *   PcaServoAngle(0, 90);          单舵机 90°
 *   OttoPcaWalk();                 默认参数走 4 步
 *   OttoPcaWalkEx(2, 800, -1, 0);  自定义:后退 2 步
 *
 * 接线(与内核版一致):
 *   PCA9685 VCC -> 板子 3.3V;GND -> 板子 GND
 *   SDA/SCL     -> 板上暴露的 I2C6 接口(Y18=GPIO3_2=I2C6_SCL,AA18=GPIO3_3=I2C6_SDA,
 *                  pin_config.hcs 开机已复用好,不用自己切)
 *   V+          -> 舵机独立 5V 电源正(建议 2A 以上);端子排 GND -> 电源地
 *   外部电源地必须与板子 GND 共地,否则 I2C 和信号都不通
 *   舵机三线:棕/黑 -> CHx 排针 GND;红 -> V+;橙/黄/白 -> CHx 的 PWM 针
 *
 * 说明:I2C6 与板载音频功放 TFA9879 共用(7 位地址 0x6D,PCA9685 默认 0x40,
 *       地址不冲突;HDF 对同一总线传输自动加锁串行化,互不干扰)。
 */
#ifndef PCA9685_SDK_H
#define PCA9685_SDK_H

#include "stdint.h"
#include "hdf_base.h"       /* HDF_SUCCESS / HDF_FAILURE */

#define PCA_BUS_DEFAULT    6        /* 板上暴露的 I2C6(与 TFA9879 共用) */
#define PCA_ADDR_DEFAULT   0x40     /* A0~A5 全接地时的 7 位地址 */
#define PCA_CH_NUM         16       /* PCA9685 共 16 路 PWM */
#define PCA_SERVO_CH_MAX   15       /* 通道号上限 */

/* ---- 初始化(业务代码入口) ----
 * PcaInit:一键初始化(默认总线/地址),幂等:已就绪直接成功,可重复调用;
 * PcaInitEx:指定总线/器件地址(A0~A5 跳线改过地址时用)。
 * 失败(器件无应答/总线打不开)返回 HDF_FAILURE,可直接重试 */
int32_t PcaInit(void);
int32_t PcaInitEx(uint8_t bus, uint8_t addr);

/* 底层:打开总线 + 初始化芯片;已就绪且 bus/addr 未变时直接返回成功,
 * force=1 强制重新初始化(业务一般用 PcaInit 即可) */
int32_t PcaEnsureReady(uint8_t bus, uint8_t addr, int32_t force);

/* 扫描总线上有应答的器件(0x03~0x77),打印地址,返回找到的个数(-1=总线打不开) */
int32_t PcaScanBus(uint8_t bus);

/* 舵机控制:角度 0~180(对应 0.5ms~2.5ms 脉宽)/ 直接给脉宽 us */
int32_t PcaServoAngle(uint8_t ch, uint32_t deg);
int32_t PcaServoUs(uint8_t ch, uint32_t us);

/* 全通道输出清零(ON=0/OFF=0,无脉冲输出,舵机失力松弛) */
int32_t PcaAllChannelsOff(void);

/* 查询当前状态(本进程内的就绪态;跨进程不共享) */
void PcaGetStatus(uint8_t *bus, uint8_t *addr, int32_t *ready);

#endif /* PCA9685_SDK_H */

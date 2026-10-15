/*
 * pca_port.h - 用户态平台垫片(SDK库包 专用)
 *
 * 内核版(device/soc/.../mytest/pca9685)依赖 los_task.h/los_printf.h 的
 * LOS_TaskDelay/LOS_TickCountGet/PRINTK;用户态 SDK 在这里换成 musl 等价
 * 实现,上层 API(pca9685.h / otto_pca.h)保持与内核版完全同名同参。
 */
#ifndef PCA_PORT_H
#define PCA_PORT_H

#include <stdint.h>
#include <stdio.h>
#include <time.h>

#define PRINTK printf   /* 与内核版打印风格对齐,输出到串口0 控制台 */

/* 毫秒级睡眠(等价内核 LOS_TaskDelay;被信号打断后继续睡剩余时间) */
static inline void PcaDelayMs(int32_t ms)
{
    struct timespec ts;

    if (ms <= 0) {
        return;
    }
    ts.tv_sec = ms / 1000;
    ts.tv_nsec = (long)(ms % 1000) * 1000000L;
    while (nanosleep(&ts, &ts) == -1) {
    }
}

/* 毫秒单调时钟(等价内核 LOS_TickCountGet,tick=1ms;uint32 回绕算术与内核一致) */
static inline uint32_t PcaTickMs(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)((uint64_t)ts.tv_sec * 1000U + (uint32_t)ts.tv_nsec / 1000000U);
}
#endif /* PCA_PORT_H */

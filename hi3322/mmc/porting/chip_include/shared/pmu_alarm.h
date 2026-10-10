/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2025-2025. All rights reserved.
 * Description: pmu alarm header file
 */

#ifndef PMU_ALARM_H
#define PMU_ALARM_H

#include <stdbool.h>
#include "errcode.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef int alarm_handle_t;                        // 表示alarm句柄
typedef void (*alarm_callback_t)(alarm_handle_t);  // 回调，参数表示触发的handle

#define MAX_ALARM_NUM (5)  // 逻辑闹钟数量上限，单次闹钟和按周配置均有这些数量的闹钟
#define MAX_ALARM_HANDLE (MAX_ALARM_NUM * 2)  // 最大alarm handle，handle范围：[0, MAX_ALARM_HANDLE-1]

/* Alarm Error Code */
#define ERRCODE_ALARM_NOT_INIT                              0x80001030
#define ERRCODE_ALARM_NOT_ENOUGH                            0x80001031
#define ERRCODE_ALARM_NOT_CLOSE                             0x80001032
#define ERRCODE_ALARM_NOT_RESTART                           0x80001033
#define ERRCODE_ALARM_NOT_DEL                               0x80001034
#define ERRCODE_ALARM_CFG_PATH_NOT_EXIST                    0x80001035

typedef enum {
    ALARM_MSG_TYPE_BEGIN = 0x1000,
    CLOCK_CALIBRATION = ALARM_MSG_TYPE_BEGIN,
} alarm_msg_type_t;

typedef union {
    struct {
        uint8_t sun : 1;
        uint8_t mon : 1;
        uint8_t tue : 1;
        uint8_t wed : 1;
        uint8_t thu : 1;
        uint8_t fri : 1;
        uint8_t sat : 1;
        uint8_t reserved : 1;
    } days;
    uint8_t raw;
} week_days_t;

typedef struct {
    week_days_t week_days;  // 可以选择一周的任意天，如果都不选择，则是一个不重复闹钟
    uint8_t hour;
    uint8_t min;
    uint8_t reserved;
} alarm_week_time_t;

typedef enum {
    ALARM_SINGLE = 0,       // 单次闹钟
    ALARM_WEEK,             // 按周配置的闹钟
} alarm_type_t;

typedef enum {
    ALARM_IDLE = 0,     // 空闲
    ALARM_CLOSE,        // 关闭
    ALARM_ENABLE,       // 启用
} alarm_status_t;

typedef struct {
    uint64_t alarm_timestamp;           // 到期timestamp
    alarm_week_time_t alarm_week_time;  // 按周配置的闹钟参数
    alarm_status_t status;              // 闹钟状态，空闲时其他值无效
    alarm_type_t type;                  // 闹钟类型
} alarm_info_t;

// 应用类型，业务自行添加
typedef enum {
    ALARM_APP_SERVICE = 0,
    // other app

    ALARM_APP_MAX_NUM,  // 最大数量，在其之前添加
} alarm_app_t;

/**
 * 注册闹钟回调
 * @param app_type 应用类型，不同业务注册不同的type，以实现通知多业务
 * @param callback 闹钟回调函数
 * @return errcode_t 错误码
 */
errcode_t uapi_alarm_register_callback(alarm_app_t app_type, alarm_callback_t alarm_callback);

/**
 * 初始化闹钟接口，每次上电启动必须调用
 * @return errcode_t 错误码
 */
errcode_t uapi_alarm_init(void);

/**
 * 反初始化闹钟接口
 * @return errcode_t 错误码
 */
errcode_t uapi_alarm_deinit(void);

/**
 * 创建一个单次的闹钟，到期后会自动删除
 * 仅在开机时有效，关机后会清理
 * close和delete均会关闭并删除该闹钟，且不允许被restart
 * 注意：内部实现存储了到期的时间戳，如果向后修改时间导致闹钟过期，将不会被触发
 * @param handle 返回的闹钟句柄
 * @param seconds 从现在开始到闹钟触发的秒数
 * @return errcode_t 错误码
 */
errcode_t uapi_alarm_create(alarm_handle_t *handle, uint32_t seconds);

/**
 * 创建一个按星期配置的周期闹钟，删除需要调用delete接口
 * 支持创建重复闹钟和不重复闹钟，不重复的闹钟将在触发一次后关闭，配置方式参见alarm_week_time_t
 * @param handle 返回的闹钟句柄
 * @param time 闹钟触发的时间
 * @return errcode_t 错误码
 */
errcode_t uapi_alarm_period_create(alarm_handle_t *handle, alarm_week_time_t alarm_week_time);

/**
 * 关闭一个闹钟，关闭后仍然占用闹钟用量，但不会被调度
 * @param handle 返回的闹钟句柄
 * @return errcode_t 错误码
 */
errcode_t uapi_alarm_close(alarm_handle_t handle);

/**
 * 重新启用一个已关闭的闹钟
 * 注意：只能启用按周配置的闹钟
 * @param handle 返回的闹钟句柄
 * @return errcode_t 错误码
 */
errcode_t uapi_alarm_restart(alarm_handle_t handle);

/**
 * 删除一个闹钟
 * @param handle 要删除的闹钟句柄
 * @return errcode_t 错误码
 */
errcode_t uapi_alarm_delete(alarm_handle_t handle);

/**
 * 查询所有闹钟，如果闹钟状态为空闲，则其他参数无效
 * @param alarm_infos 输入的数组指针
 * @param alarm_size 数组长度，应当等于MAX_ALARM_HANDLE，以返回所有闹钟
 * @return errcode_t 错误码
 */
errcode_t uapi_alarm_query(alarm_info_t *alarm_infos, uint32_t alarm_size);

/**
 * irq函数
 */
int alarm_irq_handler(int irq_num, const void *tmp);

/**
 * 时间同步，同步时间差值，单位：秒
 */
void alarm_sync_timestamp(uint64_t timestamp);

/**
 * alarm导致的唤醒，通知alarm再init后回调
 */
void trigger_alarm(void);

/**
 * shipmode前，进行alarm处理
 */
void alarm_before_shipmode(void);

/**
 * 通知队列处理任务
 * msg_type：消息类型
 */
errcode_t notify_alarm(int msg_type);

/**
 * @}
 */
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
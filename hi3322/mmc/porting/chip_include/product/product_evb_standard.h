/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  product tws common config
 * Author: @CompanyNameTag
 * Create:  2021-05-14
 */
#ifndef PRODUCT_FPGA_CONFIG_H
#define PRODUCT_FPGA_CONFIG_H

#include "chip_core_definition.h"

/**
 * @addtogroup PRODUCT
 * @{
 */
/* Platform board config, need check if it support or not carefully. */
#define BTH_WITH_SMART_WEAR                 YES
#define APP_HEAP_SIZE                       0x18000    // the minimum APP heap size (96kB)
#define WAIT_APPS_DUMP_FOREVER              NO
#if CORE == BT
#ifdef DEVICE_ONLY
#define ENABLE_LOW_POWER                    NO
#else
#define ENABLE_LOW_POWER                    YES // YES
#endif
#else
#ifdef WSTP_LPCI_ENABLE
#define ENABLE_LOW_POWER                    YES
#else
#define ENABLE_LOW_POWER                    NO
#endif
#endif
#define GLB_RTC_BASE_ADDR                   0x57017000 // 全系统RTC模块基地址
// 分区表-页为单位-大小4k 开始
#define SSB_IMAGE_PAGES                     16
#define SSB_BACKUP_IMAGE_PAGES              16
#define RECOVERY_IMAGE_PAGES                95
#define SELITEOS_IMAGE_PAGES                64
#define BT_ROM_IMAGE_PAGES                  96      // 384KB
#define NV_IMAGE_PAGES                      4
#define FOTA_IMAGE_PAGES                    4
#define UPG_FLASHBOOT_IMAGE_PAGES           16      // 64KB
#define UPG_SELITEOS_IMAGE_PAGES            64      // 256KB
#define NV_PAGES                            4
#define NV_BACKUP_PAGES                     2
#define DFX_STORE_PAGES                     3

#ifdef LOAD_IMAGE_FROM_FS
#define BT_IMAGE_PAGES                      0
#define HIFI0_IMAGE_PAGES                   0
#define HIFI0_CODEC_PAGES                   0
#else
#define BT_IMAGE_PAGES                      120     // 480KB
#ifdef USE_PSRAM_INSTEAD_NORFLASH_FOR_CODETEXT
#define HIFI0_IMAGE_PAGES                   64      // 256KB
#define HIFI0_CODEC_PAGES                   208     // 832KB
#else
#define BT_IMAGE_PAGES                      120     // 480KB
#define HIFI0_IMAGE_PAGES                   512     // 1.5MB
#define HIFI0_CODEC_PAGES                   0
#endif
#endif

#ifdef CFG_FLASH_SPECS_8M
#define APP_IMAGE_TOTAL_PAGES               1763    // 6.89MB
#define APP_IMAGE_PAGES                     1763    // 6.89MB
#elif defined (CFG_FLASH_SPECS_16M)
#ifdef USE_PSRAM_INSTEAD_NORFLASH_FOR_CODETEXT
#define APP_IMAGE_TOTAL_PAGES               1438    // 5752KB
#define APP_IMAGE_PAGES                     1438    // 5752KB
#else
#define APP_IMAGE_TOTAL_PAGES               3811    // 14.9MB
#define APP_IMAGE_PAGES                     2176    // 8.5MB
#endif
#else
#define APP_IMAGE_TOTAL_PAGES               3179    // 12.42MB
#define APP_IMAGE_PAGES                     2048    // 8MB
#endif
#define APP_RESERVED_IMAGE_PAGES            (APP_IMAGE_TOTAL_PAGES - APP_IMAGE_PAGES)

// 分区表-页为单位-大小4k 结束
#define OSC_EN_CALLBACK_BY_PLT              YES
#define BT_MIPS_DEBUG                       NO
#define NON_OS_CRITICAL_RECORD              NO
#define ENABLE_MASSDATA_RECORD              NO
#define COMPRESS_LOG_TRIGGER_THRESHOLD      0
#define COMPRESS_LOG_COUNT_THRESHOLD        0xFFFFFFFF
#define EXCEPTION_TEST_ENABLE               YES
#define LOG_LEVEL_APP_DEFAULT_CONFIG        3   // Info level
#define LOG_LEVEL_BT_DEFAULT_CONFIG         3   // Info level
#define LOG_LEVEL_DSP_DEFAULT_CONFIG        3   // Info level
#define BCPU_HEAP_MININUM_SIZE              0xA000 // 40KB
#define SYS_DEBUG_MODE_ENABLE               YES
#define AUDIO_DATA_STREAM_REGION_LENGTH     0x10000
#define APP_MASSDATA_LENGTH                 0x800
#define BT_MASSDATA_LENGTH                  0x400
#define BT_LOGGING_LENGTH                   0x1C00
#define APP_LOGGING_LENGTH                  0xC00
#define DSP_LOGGING_LENGTH                  0x2000
#define CH_BT_REGION_LEN                    1024
#define CH_APP_REGION_LEN                   1024
/********************Other module board config********************/
#define BTH_WEAR_ENABLE_AUDIO_SINK          NO
#define BTH_WEAR_ENABLE_AUDIO_GATEWAY       NO
#define BTH_WEAR_BREDR_DOUBLE_CONNECT       NO
#define BTC_DFX_LOG_HELP_SUPPORT            NO
#define BTH_WEAR_ENABLE_CONNECT_MANAGER     NO
#define BTH_WEAR_ENABLE_BLE_FEATURES        YES
#define BTH_WEAR_ENABLE_HFP_FEATURES        NO
#define BTH_HIGH_POWER                      YES
#define MULTI_CONNECT                       YES
#define BTH_CONFIG_HDAP                     YES
#define DEVICE_MANAGE_FEATURE               YES
#define BTH_CALL_LC3_32K                    YES
#define BTH_ENABLE_LC3_CODEC                YES
#define ENABLE_CHANGE_DEVICE_NAME           YES
#define BTH_DIP_PRODUCT_ID                  0x4106
#define BTH_ENABLE_L2HC_CODEC               YES
#define BTH_WSTP_CMD_SUPPORT                YES
#define BT_MANAGER_DEPLOYED                 2
#define BT_CODEC_TID                        2
#define BTH_WEAR_ENABLE_AVRCP_TARGET        NO
#define BTH_WEAR_ENABLE_AVRCP_CONTROLLER    NO
#define BTH_WEAR_ENABLE_AUDIO_SOURCE        YES
#define BTH_WEAR_ALLOW_INQUIRY_SCAN         YES
#define BTH_WEAR_ENABLE_HALL_STATE          NO
#define BTH_DISABLE_AAC_CODEC               YES
#define USE_COMPRESS_LOG_INSTEAD_OF_SDT_LOG NO

// 音频内置输入设备
#define BUILTIN_AI_PORT                            0x50  // 参考 soc_uapi_ai.h 中 UAPI_AI_PORT_LPADC0 枚举
#define BUILTIN_AI_PORT_ATTR_ADC_RX_TYPE           0     // 参考 soc_uapi_ai.h 中 UAPI_AI_RX_DEFAULT 枚举
#define BUILTIN_AI_PCM_ATTR_AUDIO_CH               1     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_CHANNEL_1 枚举
#define BUILTIN_AI_PCM_ATTR_BIT_DEPTH              16    // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_BIT_DEPTH_16 枚举
#define BUILTIN_AI_PCM_ATTR_SAMPLE_RATE            16000 // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_SAMPLE_RATE_16K 枚举
#define BUILTIN_AI_PCM_ATTR_FRAME_PER_SEC          100   // 每秒帧数

// 音频内置输出设备
#define BUILTIN_SND_OUT_PORT                       0x00  // 参考 soc_uapi_sound.h 中 UAPI_SND_OUT_PORT_DAC0 枚举
#define BUILTIN_SND_OUT_PORT_NUM                   1     // 端口个数
#define BUILTIN_SND_OUT_AUDIO_CH                   2     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_CHANNEL_2 枚举
#define BUILTIN_SND_OUT_BIT_DEPTH                  16    // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_BIT_DEPTH_16 枚举
#define BUILTIN_SND_OUT_SAMPLE_RATE                48000 // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_SAMPLE_RATE_48K 枚举
#define BUILTIN_SND_OUT_I2S_ATTR_I2S_BCLK          8     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_BCLK_8_DIV 枚举
#define BUILTIN_SND_OUT_I2S_ATTR_BIT_DEPTH         16    // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_BIT_DEPTH_16 枚举
#define BUILTIN_SND_OUT_I2S_ATTR_AUDIO_CH          2     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_CHANNEL_2 枚举
#define BUILTIN_SND_OUT_I2S_ATTR_I2S_STD_MODE      0     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_STD_MODE 枚举
#define BUILTIN_SND_OUT_I2S_ATTR_MASTER            1     // 1:主设备,0:从设备
#define BUILTIN_SND_OUT_I2S_ATTR_I2S_MCLK          1     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_MCLK_256_FS 枚举
#define BUILTIN_SND_OUT_I2S_ATTR_I2S_PCM_DELAY     1     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_PCM_1_DELAY 枚举
#define BUILTIN_SND_OUT_I2S_ATTR_SAMPLE_RISE_EDGE  1     // 参考 soc_uapi_audio.h 中 pcm_sample_rise_edge 成员

// 音频外挂输入设备
#define EXTERNAL_AI_PORT                           1     // 参考 soc_uapi_ai.h 中 UAPI_AI_PORT_I2S1 枚举
#define EXTERNAL_AI_I2S_ATTR_MASTER                1     // 1:主设备,0:从设备
#define EXTERNAL_AI_I2S_ATTR_I2S_MODE              0     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_STD_MODE 枚举
#define EXTERNAL_AI_I2S_ATTR_MCLK                  4     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_MCLK_768_FS 枚举
#define EXTERNAL_AI_I2S_ATTR_BCLK                  24    // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_BCLK_24_DIV 枚举
#define EXTERNAL_AI_I2S_ATTR_BIT_DEPTH             16    // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_BIT_DEPTH_16 枚举
#define EXTERNAL_AI_I2S_ATTR_AUDIO_CH              2     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_CHANNEL_2 枚举
#define EXTERNAL_AI_I2S_ATTR_SAMPLE_RISE_EDGE      1     // 参考 soc_uapi_audio.h 中 pcm_sample_rise_edge 成员
#define EXTERNAL_AI_I2S_ATTR_PCM_DELAY_CYCLE       1     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_PCM_1_DELAY 枚举
#define EXTERNAL_AI_PCM_ATTR_CHANNELS              2     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_CHANNEL_2 枚举
#define EXTERNAL_AI_PCM_ATTR_BIT_DEPTH             16    // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_BIT_DEPTH_16 枚举
#define EXTERNAL_AI_PCM_ATTR_SAMPLE_RATE           16000 // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_SAMPLE_RATE_16K 枚举
#define EXTERNAL_AI_PCM_ATTR_FRAME_PER_SEC         100   // 每秒帧数

// 音频外挂输出设备
#define EXTERNAL_SND_OUT_PORT                      0x10  // 参考 soc_uapi_sound.h 中 UAPI_SND_OUT_PORT_I2S0 枚举
#define EXTERNAL_SND_OUT_AUDIO_CHANNEL             2     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_CHANNEL_2 枚举
#define EXTERNAL_SND_OUT_AUDIO_BIT_DEPTH           16    // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_BIT_DEPTH_16 枚举
#define EXTERNAL_SND_OUT_AUDIO_SAMPLE_RATE         48000 // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_SAMPLE_RATE_48K 枚举
#define EXTERNAL_SND_OUT_PORT_NUM                  1     // 端口个数
#define EXTERNAL_SND_OUT_I2S_ATTR_BIT_DEPTH        16    // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_BIT_DEPTH_16 枚举
#define EXTERNAL_SND_OUT_I2S_ATTR_CHANNEL          2     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_CHANNEL_1 枚举
#define EXTERNAL_SND_OUT_I2S_ATTR_I2S_MODE         0     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_STD_MODE 枚举
#define EXTERNAL_SND_OUT_I2S_ATTR_MCLK             3     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_MCLK_512_FS 枚举
#define EXTERNAL_SND_OUT_I2S_ATTR_MASTER           1     // 1:主设备,0:从设备
#define EXTERNAL_SND_OUT_I2S_ATTR_SAMPLE_RISE_EDGE 1     // 参考 soc_uapi_audio.h 中 pcm_sample_rise_edge 成员
#define EXTERNAL_SND_OUT_I2S_ATTR_BCLK             16    // 参考 soc_uapi_audio.h 中 uapi_audio_i2s_bclk_sel 成员
#define EXTERNAL_SND_OUT_I2S_ATTR_I2S_PCM_DELAY    1     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_I2S_PCM_1_DELAY 枚举

// TP CONFIG
// TP CONFIG
#define TOUCH_I2C_BUS I2C_BUS_1
#define TOUCH_INT_GPIO S_AGPIO0
#define TOUCH_RESET_GPIO S_AGPIO6

// lcd gpio
#define VCI_LCD_GPIO                        S_AGPIO20
#define RESET_LCD_GPIO                      S_AGPIO27
#define DISPLAY_TE_GPIO                     S_AGPIO19

// Emmc一共四个分区，前三个分区大小固定，最后一个user分区使用剩余空间。
#define EMMC_VOLUME_BOOT_SECTOR_COUNT       12000ULL
#define EMMC_VOLUME_SYS_SECTOR_COUNT        3145666ULL
#define EMMC_VOLUME_INNER_SECTOR_COUNT      262144ULL
#define EMMC_VOLUME_SYS_BK_SECTOR_COUNT     524288ULL

// TRACK MODE
#define SND_TRACK_MODE_BT_MUSIC             0 // 参考 soc_uapi_sound.h 中 UAPI_SND_TRACK_STEREO 枚举
#define SND_TRACK_MODE_BT_SCO               1 // 参考 soc_uapi_sound.h 中 UAPI_SND_TRACK_DOUBLE_MONO 枚举
#define SND_TRACK_MODE_LOCAL_CAT1           1 // 参考 soc_uapi_sound.h 中 UAPI_SND_TRACK_DOUBLE_MONO 枚举
#define SND_TRACK_MODE_BT_CAT1              0 // 参考 soc_uapi_sound.h 中 UAPI_SND_TRACK_STEREO 枚举
#define SND_TRACK_MODE_LOCAL_MUSIC          1 // 参考 soc_uapi_sound.h 中 UAPI_SND_TRACK_DOUBLE_MONO 枚举

// AEF

#define SND_PORT_AEF_TYPE                   0x9 // 参考 soc_uapi_aef.h 中 UAPI_AEF_TYPE_HVS 枚举
#define SND_PORT_AEF_ENABLE                 true // true代表开启aef算法，false代表关闭aef算法

// AI BITRATE
#define OPUS_ENC_BIT_RATE 					16000 // opus编码码率
#define MP3_ENC_BIT_RATE 					64 // mp3编码码率，单位kbps

// volume
#define DEFAULT_AI_VOL_DB_INTERGER 		    3    // 默认的AI的音量值整数部分，单位DB

// AAC MPEG4 decode support
#define CONFIG_AAC_MPEG4_SUPPORT            0     // 默认0 设置为1后支持AAC MPEG4编码格式的码流解码

// AHE lib ai attr
#define AUDIO_AHE_AI_ATTR_CH               1     // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_CHANNEL_1 枚举
#define AUDIO_AHE_AI_ATTR_BIT_DEPTH        32    // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_BIT_DEPTH_32 枚举
#define AUDIO_AHE_AI_ATTR_SAMPLE_RATE      48000 // 参考 soc_uapi_audio.h 中 UAPI_AUDIO_SAMPLE_RATE_48K 枚举
#define AUDIO_AHE_AI_ATTR_FRAME_PER_SEC    200   // 每秒帧数
#define BUILTIN_AI_PCM_ATTR_SAMPLE_PER_FRAME (BUILTIN_AI_PCM_ATTR_SAMPLE_RATE / BUILTIN_AI_PCM_ATTR_FRAME_PER_SEC)
#define AUDIO_AHE_AI_ATTR_SAMPLE_PER_FRAME (AUDIO_AHE_AI_ATTR_SAMPLE_RATE / AUDIO_AHE_AI_ATTR_FRAME_PER_SEC)

/**
 * @} end of group PRODUCT
 */
#endif

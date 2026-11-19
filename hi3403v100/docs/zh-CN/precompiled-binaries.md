# 基础能力依赖说明

## 基础能力说明

| 分类 | 库名称 | 存放路径 | 主要功能 |
| --- | --- | --- | --- |
| 媒体 | libmpp_media.so | `hardware/lib` | 媒体处理基础框架:OS 抽象(OSAL/BASE/SYS)、中断、视频 IO(VI/VO/VPSS/VPP/VGS)、区域(RGN)、去雾(dehaze)、平台 MPI(ss_mpi)、HDMI 输出、2D 图形加速(TDE)、片间视频(PCIV)、PQ 画质处理(PQP) |
| 编解码 | libmpp_codec.so | `hardware/lib` | 视频编解码:通道管理、H.264/H.265 编码、JPEG 编解码、码率控制(RC)、解码(VDEC/VFMW)、编码(VENC/VEDU) |
| ISP | libmpp_isp.so | `hardware/lib` | 图像信号处理:ISP 框架与 MPI、防抖(DIS)、畸变校正(GDC)、3A(AE/AWB)、降噪(BNR)、抗闪、扩展统计、CRB/DRC/LDCI/红外算法、传感器接口(I2C/SPI) |
| 音频 | libmpp_audio.so | `hardware/lib` | 音频全栈:AAC 编解码(含 SBR)、音频输入输出(AI/AO/AIO)、音频编解码器(AENC/ADEC/ACODEC)、语音增强(VQE 已并入本库) |
| 安全 | libmpp_sec.so | `hardware/lib` | 安全子系统:Cipher 加解密、Klad 密钥阶梯、OTP 一次性编程 |
| SVP/AI | libmpp_ai.so、libsvp_aicpu.so | `hardware/lib` | SVP ACL 运行库与 AICPU 算子库，仅 `mindspore_feature_svp=true` 时由 `mindspore_ndk` 链接；NPU 内核模块 ot_svp_npu.ko 的驱动源码不在本仓 |

## U-Boot 启动固件（bin）

| 文件 | 主要功能 |
| --- | --- |
| fip.bin | ARM Trusted Firmware-A（ATF）固件镜像包（FIP）。Hi3403V100 为 ARMv8-A 64 位架构，安全启动需集成 ATF；该镜像与 uImage 划分独立分区烧写 |
| boot_image_4GB.bin | 4GB DDR 配置下的系统启动镜像（U-Boot），已按 fip.bin 与 uImage 独立分区的烧写方案适配 |

当前仅提供 4GB DDR 配置的固件（配套环境变量文件 `bootloader/uboot_env_4GB.txt`）。fip.bin 与 uImage 的烧写分区划分依赖板级烧写配置，详见[开发板使用教程](https://gitcode.com/openharmony/device_board_hisilicon/blob/master/hispark_aifly/README_zh.md)的烧录说明章节。

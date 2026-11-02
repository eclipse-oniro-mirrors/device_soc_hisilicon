# 基础能力依赖说明

## 基础能力说明

预编译库随仓发布，位于 `sdk_linux/smp/a55_linux/mpp/out/lib`，按功能分类如下：

| 分类 | 库名称 | 主要功能 |
| --- | --- | --- |
| 媒体 | libss_mpi.so、libot_base.so、libot_sys.so、libot_vi.so、libot_vo.so、libot_vpss.so、libot_vgs.so、libot_rgn.so 等 | 媒体处理基础框架:OS 抽象(OSAL/BASE/SYS)、中断、视频 IO(VI/VO/VPSS/VPP/VGS)、区域(RGN)、去雾(dehaze，libss_dehaze.so)、HDMI 输出(libss_hdmi.so)、2D 图形加速(TDE，libss_tde.so)、片间视频(PCIV，libss_pciv.so)、PQ 画质处理(PQP，libss_pqp.so) |
| 编解码 | libot_chnl.so、libot_venc.so、libot_h264e.so、libot_h265e.so、libot_jpege.so、libot_jpegd.so、libot_rc.so、libot_vdec.so、libot_vfmw.so、libot_vedu.so | 视频编解码:通道管理、H.264/H.265 编码、JPEG 编解码、码率控制(RC)、解码(VDEC/VFMW)、编码(VENC/VEDU) |
| ISP | libss_isp.so、libot_isp.so、libot_mpi_isp.so、libss_ae.so、libss_awb.so、libss_bnr.so 等 | 图像信号处理:ISP 框架与 MPI、防抖(DIS)、畸变校正(GDC)、3A(AE/AWB)、降噪(BNR)、抗闪、扩展统计、CRB/DRC/LDCI/红外算法、传感器接口(I2C/SPI) |
| 音频 | libaac_*.so、libot_aio.so、libot_ai.so、libot_ao.so、libot_aenc.so、libot_adec.so、libot_acodec.so、libvqe_*.so、libss_dnvqe.so、libss_upvqe.so 等 | 音频全栈:AAC 编解码(含 SBR)、音频输入输出(AI/AO/AIO)、音频编解码器(AENC/ADEC/ACODEC)、语音增强(VQE) |
| 安全 | libss_cipher.so、libss_klad.so、libss_otp.so、libss_acs.so | 安全子系统:Cipher 加解密、Klad 密钥阶梯、OTP 一次性编程 |

### 集成方式与依赖关系

上述预编译库编译时由 `sdk_linux/build.sh` 统一拷贝到镜像 out lib 目录并随文件系统打包。这些库为 HAL 层与 Sensor 驱动提供底层能力：Sensor 驱动（`sdk_linux/smp/a55_linux/mpp/sample/` 下各工程）依赖 `libss_awb.so`、`libss_ae.so`、`libot_mpi_isp.so` 等 ISP 库，媒体 HAL 依赖 VI/VPSS/VENC 等媒体库，安全相关能力（Cipher 加解密、Klad、OTP）由 `libss_cipher.so`、`libss_klad.so`、`libss_otp.so` 提供。

## U-Boot 启动固件（bin）

| 文件 | 主要功能 |
| --- | --- |
| fip.bin | ARM Trusted Firmware-A（ATF）固件镜像包（FIP）。Hi3403V100 为 ARMv8-A 64 位架构，安全启动需集成 ATF；该镜像与 uImage 划分独立分区烧写 |
| boot_image_4GB.bin | 4GB DDR 配置下的系统启动镜像（U-Boot），已按 fip.bin 与 uImage 独立分区的烧写方案适配 |

当前仅提供 4GB DDR 配置的固件（配套环境变量文件 `uboot/uboot_env_4GB.txt`）。fip.bin 与 uImage 的烧写分区划分依赖板级烧写配置，详见[开发板使用教程](https://gitcode.com/openharmony-sig/device_board_hisilicon_hispark_aifly/blob/master/hispark_aifly/README_zh.md)的烧录说明章节。

# Hi3403V100

- [简介](#简介)
- [目录结构](#目录结构)
- [基础能力](#基础能力)
- [高阶功能](#高阶功能)
- [典型产品](#典型产品)
- [外设开发](#外设开发)
- [约束](#约束)
- [许可协议](#许可协议)
- [相关仓](#相关仓)

## 简介

Hi3403V100 是上海海思推出的高性能智能视觉芯片。本目录为 HiSpark AiFly 开发板提供芯片侧软件，向下封装芯片驱动与海思预编译二进制，向上为 OpenHarmony 的图形、媒体、AI、安全子系统提供南向实现，覆盖音视频采集与编解码、视频前处理、封装解封装、存储管理与日志等能力。系统类型为小型系统（Small System）。

### 主控芯片介绍

Hi3403V100 是一颗面向视觉领域推出的专业 Ultra-HD Smart IP Camera SoC。该芯片最高支持四路 Sensor 输入，支持最高 4K60 的 ISP 图像处理能力，支持 3F WDR、多级降噪、六轴防抖、硬件拼接等多种图像增强和处理算法。Hi3403V100 内置四核 ARM Cortex-A55@1.4GHz 处理器，提供高效且灵活的 CPU 资源；内置 32bit MCU，以满足低延时要求较高的场景。芯片集成了高效的神经网络推理单元，最高 10.4TOPS INT8 算力，支持业界主流神经网络框架；内置双核 Vision DSP，以满足差异化的 CV 计算需求。Hi3403V100 采用先进低功耗工艺和封装，支持 LPDDR4/LPDDR4x/DDR4 颗粒，满足产品小型化设计和快速量产需求。

### 核心特性

| 特性类别 | 核心能力 |
| --- | --- |
| 处理器内核 | 四核 ARM Cortex-A55@1.4GHz（支持 Neon 加速、集成 FPU），内置 32bit MCU@500MHz，支持 TrustZone |
| 智能视频分析 | 图像分析加速引擎高达 10.4TOPS@INT8（双内核异构引擎），双核 Vision Q6 DSP，内置智能计算加速、双目深度加速、矩阵计算加速单元 |
| 视频编解码 | H.264/H.265 编解码最高分辨率 8192×8192，支持 4K60 编码和 10 路 1080p30 解码，支持 CBR/VBR/AVBR/FIXQP/QPMAP 等多种码率控制模式 |
| 视频输入 | 支持 8-Lane image sensor 串行输入（MIPI/LVDS/Sub-LVDS/HiSPi），最高支持 4 路 Sensor 输入，最大分辨率 8192×8192 |
| 数字图像处理 ISP | 支持 3A（AE/AWB/AF）功能，支持 3F WDR、多级 3D 去噪、去雾、动态对比度增强、镜头畸变校正、6-DoF 数字防抖、超感光去噪（HNR）等 |
| 视频输出 | 支持 HDMI2.0、4-Lane MIPI DSI/CSI、CVBS 等多种输出接口，最大输出能力 4096×2160@60fps + 1920×1080@60fps |
| 外部存储 | 支持 DDR4/LPDDR4/LPDDR4x（最高速率 3733Mbps，最大容量 8GB），支持 SPI Nor/SPI Nand Flash、NAND Flash、eMMC5.1 |
| 外围接口 | 2 个千兆以太网、2 个 USB3.0/USB2.0、2-Lane PCIe2.0、2 个 SDIO3.0、多个 UART/I²C/SPI/GPIO 接口 |
| 安全能力 | OTP 一次性可编程存储（安全启动根公钥哈希、多组对称根密钥、SoC 特性开关、约 25Kbit 用户区，支持版本防回滚）；基于 OTP 根密钥的三级密钥派生与加扰保护；SPACC 密码加速引擎、RSA 与硬件真随机数；基于硬件可信根的安全启动与 TrustZone 可信执行环境；经 `hal_huks` 对接 OpenHarmony HUKS |

## 目录结构

```bash
├── docs/zh-CN                  # 专题文档
├── hdf_driver                  # HDF 驱动适配（display、mipi_dsi）
├── sdk_linux                   # 芯片 SDK
│   ├── BUILD.gn                # 编译入口 GN 文件
│   ├── build.sh                # SDK 编译入口（编译 ko、ATF 等）
│   ├── open_source             # 开源组件（linux、u-boot、trusted-firmware-a、mbedtls）
│   ├── osdrv                   # 驱动组件与工具
│   └── smp/a55_linux
│       ├── interdrv            # 芯片驱动接口（mipi_rx、mipi_tx、sysconfig 等）
│       ├── mpp                 # MPP 媒体处理平台（cbb、component、sample 等）
│       └── osal                # 系统适配层
└── uboot                       # U-Boot / ATF 固件
```

## 基础能力

- [软件架构说明](./docs/zh-CN/software-architecture.md)
- [基础能力依赖说明](./docs/zh-CN/precompiled-binaries.md)

## 高阶功能

- [高阶功能说明](./docs/zh-CN/advanced-features.md)

## 典型产品

- [开发板使用教程](https://gitcode.com/openharmony-sig/device_board_hisilicon_hispark_aifly/blob/master/hispark_aifly/README_zh.md)
- [IP摄像机](./docs/zh-CN/ipc-product.md)

## 外设开发

- [外设适配实操指南](./docs/zh-CN/porting-smallchip-peripheral.md)

## 约束

- 当前版本仅支持 Hi3403V100 芯片，系统类型为小型系统（linux-6.6 + musl）。

## 许可协议

本项目包含多种许可证，各模块遵循对应的许可协议。

### Apache License 2.0

- **自研代码**：Hi3403V100 自研代码使用 Apache License Version 2.0，版权声明：

  ```
  Copyright (c) 2025 HiSilicon (Shanghai) Technologies Co., Ltd.
  ```

### GNU General Public License v2 (GPL-2.0)

以下开源组件采用 GPL-2.0，各组件许可声明的出处见括号内说明：

- **Linux 内核**（Linux-6.6）：内核源码取自 OpenHarmony 社区仓 kernel_linux，许可遵循社区仓声明
- **U-Boot**：随 `uboot/` 以二进制形式分发，开源软件声明见本仓 `NOTICE` 中 "Das U-Boot 2020.01" 条目
- **MPP 媒体处理平台**（component、sample、cbb 等）：`sdk_linux/smp/a55_linux/mpp/` 下各模块的 `LICENSE`
- **OSAL 系统适配层**：`sdk_linux/smp/a55_linux/osal/LICENSE`
- **interdrv 外设驱动**（MIPI RX/TX、sysconfig 等）：`sdk_linux/smp/a55_linux/interdrv/LICENSE`

### BSD 3-Clause License

- **Trusted Firmware-A (TF-A)**：ARM 安全固件（源码位于 `sdk_linux/open_source/trusted-firmware-a/`，编译产物 fip.bin 随 `uboot/` 分发），采用 BSD 3-Clause。

### 兼容性说明

- 本目录包含其它开源软件组件，各组件遵循其各自的开源许可声明；
- 若开源组件许可与本项目许可冲突，以该组件许可为准；
- 使用本项目代码时，请遵守相应许可证的要求和限制。

## 相关仓

- [device_soc_hisilicon](https://gitcode.com/openharmony/device_soc_hisilicon)（本仓）
- [device_board_hisilicon](https://gitcode.com/openharmony/device_board_hisilicon)
- [vendor_hisilicon](https://gitcode.com/openharmony/vendor_hisilicon)

HiSpark AiFly 解决方案 SIG 仓：

- [device_soc_hisilicon_hispark_aifly](https://gitcode.com/openharmony-sig/device_soc_hisilicon_hispark_aifly)
- [device_board_hisilicon_hispark_aifly](https://gitcode.com/openharmony-sig/device_board_hisilicon_hispark_aifly)
- [vendor_hisilicon_hispark_aifly](https://gitcode.com/openharmony-sig/vendor_hisilicon_hispark_aifly)

# 高阶功能

除基础的音视频采集、编解码与显示能力外，本仓还提供端侧 AI 推理能力与安全能力。下表按用途列出各功能的说明与详细文档。

## 端侧 AI 推理

Hi3403V100 提供 CPU 与 NPU 两条推理路径：CPU 适合轻量模型与通用任务，NPU 面向高算力视觉模型。

| 功能 | 说明 | 文档 |
| --- | --- | --- |
| 推理方案总览 | CPU / NPU 两条路径的适用场景、框架与算力对比 | [AI 推理方案总览](./porting-smallchip-ai-overview.md) |
| 轻量模型推理（CPU） | 使用系统镜像内置的 MindSpore Lite 运行时，在小型系统上运行常规视觉模型推理 | [轻量推理框架 MindSpore Lite](./porting-smallchip-ai-CPU-MindSporeLite.md) |
| NPU 模型部署 | 以 ModelZoo 的 YOLOv9s 为例，走通模型转换到 NPU 推理 | [NPU 推理与 ModelZoo 案例](./porting-smallchip-ai-NPU-ModelZoo.md) |
| NPU 统一调用接口 | 通过 SVPDelegate 把 NPU 封装为 MindSpore Lite Delegate，用统一 C API 调用，以 ResNet50 为例 | [通过 SVPDelegate 调用 NPU](./porting-smallchip-ai-NPU-MindSporeLite-svp-delegate.md) |

## 安全子系统

| 功能 | 说明 | 文档 |
| --- | --- | --- |
| 安全子系统使用说明 | Hi3403V100 安全能力总览：OTP 结构与字段定义（密钥区域、单比特/多比特控制区域、用户自定义区域、shadow 寄存器）、基于 OTP 对称根密钥的三级密钥派生、SPACC 硬件密码加速引擎、硬件真随机数、安全启动信任链、TrustZone 与安全 JTAG | [安全子系统使用说明](https://gitee.com/HiSpark/pegasus/blob/master/docs/04API%E5%8F%82%E8%80%83/%E5%AE%89%E5%85%A8%E5%AD%90%E7%B3%BB%E7%BB%9F/%E5%AE%89%E5%85%A8%E5%AD%90%E7%B3%BB%E7%BB%9F%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%AE%89%E5%85%A8%E5%AD%90%E7%B3%BB%E7%BB%9F%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E.md) |
| 安全启动使用指南 | 基于 OTP 根公钥哈希的镜像逐级验签流程、镜像签名与加密制作方法、安全启动使能与 OTP 烧写操作步骤 | [安全子系统使用说明（安全启动章节）](https://gitee.com/HiSpark/pegasus/blob/master/docs/04API%E5%8F%82%E8%80%83/%E5%AE%89%E5%85%A8%E5%AD%90%E7%B3%BB%E7%BB%9F/%E5%AE%89%E5%85%A8%E5%AD%90%E7%B3%BB%E7%BB%9F%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E/%E5%AE%89%E5%85%A8%E5%AD%90%E7%B3%BB%E7%BB%9F%E4%BD%BF%E7%94%A8%E8%AF%B4%E6%98%8E.md#ZH-CN_TOPIC_0000002424349950) |

> 上述安全文档托管在 HiSpark/pegasus 文档仓，其产品版本覆盖 SS928/SS626/SS927 系列（SS928V100 即 Hi3403V100 的芯片 chipid），内容适用于本芯片平台。

## SDK 开发参考与 API 文档

- [HiSpark/Hi3403V100 文档仓](https://gitcode.com/HiSpark/Hi3403V100/tree/master/docs/zh-CN)

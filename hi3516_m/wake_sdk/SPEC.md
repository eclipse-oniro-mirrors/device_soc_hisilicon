# 语音唤醒移植规格说明

> **项目**：Hi3516DV300 (HiSpark Taurus) OpenHarmony LiteOS-A 小型系统
> **目标**：将 ESP32-C6 上的 microWakeWord (Luna) 语音唤醒迁移到 Hi3516DV300
> **日期**：2026-09-21
> **状态**：方案设计阶段，待评审

---

## 1. 背景与目标

### 1.1 现状

- **录音链路已打通**：`my_audio`（rootfs/bin/my_audio）已验证可用，通过 HI_MPI_AI + /dev/acodec 实现 8kHz 录音回放。
- **已完成项目**：`/code/3516/streaming_model_deployment/` 是一个在 ESP32-C6-Touch-AMOLED-2.16 上跑通的 microWakeWord 语音唤醒项目，唤醒词 "Luna"，使用 TFLM 软推理 + 自训练 streaming int8 模型。

### 1.2 目标

把 ESP 项目的语音唤醒迁移到 Hi3516DV300，做成**用户态独立程序 `my_wake`**：

- 与 `my_audio` 同层级（直接调 HI_MPI_AI + /dev/acodec）
- 16kHz 采样（模型训练采样率）
- TFLM 软推理，使用自己训练的 Luna 唤醒词
- **不依赖** ai_server / media_server / NNIE 等系统服务
- 串口打印唤醒事件（本板无显示屏，不做 UI）

### 1.3 非目标

- 不改 my_audio（独立程序，只参考不依赖）
- 不做内核态实现（明确是用户态，见第 2 节论证）
- 不使用 NNIE 硬件加速 / 树里现成 keyword_spotting SDK
- 不做 UI 显示（ESP 项目的 AMOLED 屏，Hi3516 板子没有）
- 不做功放管理（本板无 TFA9879，已验证喇叭直连 ACodec）

---

## 2. 用户态 vs 内核态论证

### 2.1 my_audio 是用户态程序

经以下证据确认 my_audio **不是内核态**：

| 证据 | 内容 | 结论 |
|---|---|---|
| `file` 输出 | `ELF 32-bit LSB pie executable, dynamically linked, interpreter /lib/ld-musl-arm.so.1` | 动态链接的用户态可执行文件，加载器是 musl libc |
| 存放位置 | `rootfs/bin/my_audio`，打包进 `rootfs_vfat.img`（根文件系统镜像） | 不在 `OHOS_Image.bin`（内核镜像）里 |
| 构建方式 | `audio/BUILD.gn:15` 用 `executable("audio")` 模板 | 不是 `kernel_module` |
| HI_MPI 性质 | `libmpi.a` 是用户态静态库（current ar archive）；`kernel/liteos_a/` 源码树搜不到 `HI_MPI_AI_GetFrame` | HI_MPI 是用户态封装层，通过系统调用触达内核 AI 驱动 |

### 2.2 系统分层

```
┌─────────────────────────────────────────────┐
│  OHOS_Image.bin (内核镜像)                    │
│  ├─ LiteOS-A 内核                            │
│  ├─ 内核驱动:AI/AO/ACodec 底层驱动            │  ← 内核态
│  │  (在 module_init, 海思 drv 层)             │
│  └─ HDF 框架                                  │
├─────────────────────────────────────────────┤  ← 用户态/内核态分界
│  rootfs_vfat.img (根文件系统)                 │
│  ├─ /bin/my_audio  ← 用户态程序               │
│  │    └─ 静态链 libmpi.a → 系统调用 → 内核驱动  │
│  ├─ /bin/media_server, /bin/ai_server ...     │
│  └─ /lib/*.so                                  │
└─────────────────────────────────────────────┘
```

### 2.3 "摆脱系统依赖"的含义

my_audio 已经摆脱了 **media_server 服务进程**的依赖（独立进程直接调 libmpi.a）。
未摆脱的是 **HI_MPI 用户态库**——但该库不需要任何后台服务运行，是一组直接打驱动的封装。
"和录音做成一个层级"即做成同类用户态独立程序。

---

## 3. 技术可行性验证

### 3.1 已验证的关键事实

| 项 | 结论 | 证据 |
|---|---|---|
| 16kHz 采样率支持 | `AUDIO_SAMPLE_RATE_16000` + `ACODEC_FS_16000` 枚举存在 | hi_comm_aio.h:94, acodec.h:25 |
| LiteOS-A 支持 C++ | Kconfig `KERNEL_CPPSUPPORT` default y；BUILD.gn `cflags_ff = ["-std=c++11"]` | kernel/extended/Kconfig:9-14 |
| lite_component.gni 支持 C++ | 定义 `cflags_ff` 时自动加 `-lstdc++` | lite_component.gni:128-130 |
| microfrontend 纯 C 无 ESP 依赖 | 18 个源文件，1739 行，grep `esp_` 为空 | components/microfrontend/ |
| TFLM 核心 ESP 依赖极少 | 排除 esp_nn/ 后零 ESP 依赖；仅 `esp/micro_time.cc` 用 esp_timer | grep esp_ tensorflow/ |
| reference 内核齐全 | add/conv/depthwise_conv/fully_connected/mul/pooling/softmax 都有 reference 实现 | kernels/*.cc |
| debug_log.cc 标准 C | `vfprintf(stderr)` + `vsnprintf`，musl libc 支持 | debug_log.cc:39 |
| flatbuffers 版本冲突 | TFLM 钉 25.9.23，OpenHarmony 树里是 25.2.10 | **必须用 TFLM 自带的** |

### 3.2 ESP 依赖替换点（仅 3 处机械替换）

| ESP 符号 | 替换为 | 位置 | 说明 |
|---|---|---|---|
| `esp_timer_get_time()` | 删除或 `LOS_TickCountGet()` | wake_word.cc（perf 统计） | 非功能必需，reference micro_time.cc 返回 0 也能跑 |
| `ESP_LOGI/ESP_LOGE` | `printf` | wake_word.cc | 日志输出 |
| `esp_err_t / ESP_OK / ESP_FAIL` | `int / 0 / -1` | wake_word.h, wake_word.cc | 返回类型 |

### 3.3 TFLM 源文件编译策略

从 `streaming_model_deployment/managed_components/espressif__esp-tflite-micro/` 整体 vendoring：

| 源文件组 | 数量 | 处理 |
|---|---|---|
| micro/*.cc（核心） | ~20 | 全编译，**用 micro_time.cc reference 版**（非 esp/） |
| kernels/*.cc（reference） | ~40 | 全编译（含 add/conv/depthwise_conv/fc/mul/pooling/softmax） |
| kernels/esp_nn/*.cc | 7 | **跳过**（ESP 加速内核，用 reference 替代） |
| kernels/internal/*.cc | ~8 | 全编译 |
| core/api/*.cc, schema/*.cc | ~5 | 全编译 |
| microfrontend/lib/*.cc | 16 | 全编译（纯 C，无 ESP 依赖） |
| third_party/flatbuffers/ | 头文件 | include（**必须用 TFLM 自带 25.9.23**） |
| third_party/gemmlowp/ | 头文件 | include |
| third_party/kissfft/ | 头文件 | include（microfrontend 用） |
| signal/ | — | **跳过**（wake_word.cc 不直接用 signal 库） |

---

## 4. 架构设计

### 4.1 程序结构

```
userspace: my_wake (rootfs/bin/my_wake, ELF executable)
  ├─ wake_main.cc          主循环: 取帧→喂前端→推理→判定→打印
  ├─ wake_capture.c        16kHz HI_MPI_AI 采集 (参考 my_audio, 独立实现)
  ├─ wake_capture.h
  ├─ wake_word.h           TFLM 推理接口 (从 ESP 项目搬, 小改)
  ├─ wake_word.cc          TFLM 推理 + 滑动窗口 (从 ESP 项目搬, 去 ESP 依赖)
  ├─ wake_word_model_data.c  Luna 模型数据 (391KB, 直接搬)
  ├─ wake_word_model_data.h  (直接搬)
  ├─ tflm/                 vendored TFLM 推理引擎 (用 reference 内核)
  └─ microfrontend/        vendored TFLM 音频前端 (18 .c, 从 components/microfrontend/)
```

### 4.2 依赖链（静态链接，与 my_audio 同方式）

```
my_wake
  ├─ libmpi.a + libdnvqe + libVoiceEngine + hive_* + libsecurec  (HI_MPI_AI, 同 my_audio)
  ├─ libstdc++ (C++ 运行时, lite_component.gni 自动加)
  └─ libm (sin/cos/lroundf, frontend 用)
```

**不依赖**：ai_server, libai_client, libnnie, media_server, TFA9879 功放

### 4.3 信号链（与 ESP 项目 bit-matched）

```
HI_MPI_AI 16kHz mono int16
  → TFLM microfrontend: 30ms window / 10ms step, 40 channels,
    125-7500 Hz, noise reduction + PCAN, log scale  → uint16
  → × 0.0390625                                     → float
  → quantise with model's own input scale/zp        → int8
  → streaming TFLite model, 3 feature slices per invoke
  → 5-slice moving average vs cutoff (0.98)
```

前端参数来自 `pymicro-features`，与 microWakeWord 训练 pipeline 一致。
`0.0390625` 缩放和 `/255` 反量化分别对应 `microwakeword/data.py` 和 `inference.py`。

### 4.4 帧参数设计（16kHz）

| 参数 | 值 | 说明 |
|---|---|---|
| 采样率 | 16000 | `AUDIO_SAMPLE_RATE_16000` |
| acodec FS | `ACODEC_FS_16000` | |
| 每帧点数 | 480 | 30ms@16kHz，对齐一次 invoke 消耗（3×160） |
| 帧字节 | 960 | 16bit 单声道 |
| 帧数缓冲 | 30 | 与 my_audio 一致 |

取一帧（480 样本）→ 喂前端 3 次（每次 160）→ 每 3 次触发 1 次推理。
等价于 ESP 项目（取 3×160=480 再喂 3 步），但更直接。

---

## 5. 文件布局与构建

### 5.1 目录结构

```
device/soc/hisilicon/common/platform/mytest/
  ├─ BUILD.gn              (修改: deps 加 "wake")
  ├─ audio/                (现有 my_audio, 不动)
  └─ wake/                 (新增)
      ├─ BUILD.gn
      ├─ wake_main.cc
      ├─ wake_capture.h
      ├─ wake_capture.c
      ├─ wake_word.h
      ├─ wake_word.cc
      ├─ wake_word_model_data.c
      ├─ wake_word_model_data.h
      ├─ tflm/              (vendored TFLM)
      └─ microfrontend/     (vendored, 从 components/microfrontend/)
```

### 5.2 BUILD.gn 设计（wake/BUILD.gn）

```gn
import("//build/lite/config/component/lite_component.gni")

sdk_liteos_root = "//device/soc/hisilicon/hi3516dv300/sdk_liteos"
mpp_lib_dir = rebase_path("$sdk_liteos_root/mpp/lib")

executable("wake") {
  output_name = "my_wake"

  sources = [
    "wake_main.cc",
    "wake_capture.c",
    "wake_word.cc",
    "wake_word_model_data.c",
  ]
  # TFLM 核心 + reference 内核(不含 esp_nn/) + microfrontend
  # 通过文件列表或 glob 收集, 见 3.3 节策略

  include_dirs = [
    "$sdk_liteos_root/include",
    "tflm",
    "tflm/third_party/flatbuffers/include",
    "tflm/third_party/gemmlowp",
    "tflm/third_party/kissfft",
    "microfrontend",
  ]

  cflags = [
    "-DTF_LITE_STATIC_MEMORY",
    "-O2",
    "-Wno-unused-parameter", "-Wno-sign-compare",
    "-Wno-missing-field-initializers", "-Wno-double-promotion",
  ]
  cflags_cc = cflags + [ "-fno-rtti", "-fno-exceptions", "-fno-threadsafe-statics" ]

  ldflags = [
    "-L$mpp_lib_dir",
    "-Wl,--start-group",
    "$mpp_lib_dir/libmpi.a",
    "$mpp_lib_dir/libdnvqe.a",
    "$mpp_lib_dir/libupvqe.a",
    "$mpp_lib_dir/libVoiceEngine.a",
    # ... (与 my_audio 同一套 VQE 库)
    "$mpp_lib_dir/libsecurec.a",
    "-Wl,--end-group",
    "-lm",
  ]
}
```

### 5.3 mytest/BUILD.gn 修改

```gn
group("mytest") {
  deps = [ "audio", "wake" ]   # 新增 "wake"
}
```

---

## 6. 主循环设计

### 6.1 wake_main.cc 伪代码

```cpp
#include "wake_capture.h"
#include "wake_word.h"
#include <math.h>

#define SAMPLES_PER_READ 480   // 30ms @ 16kHz
#define STEPS_PER_READ   3     // 3 × 10ms

static int16_t s_audio[SAMPLES_PER_READ];

static float block_rms_dbfs(const int16_t *samples, size_t count) {
    double sum = 0.0;
    for (size_t i = 0; i < count; i++) {
        double v = (double)samples[i] / 32768.0;
        sum += v * v;
    }
    double rms = sqrt(sum / count);
    return rms > 1e-9 ? (float)(20.0 * log10(rms)) : -120.0f;
}

int main(int argc, char **argv) {
    if (WakeCaptureInit() != 0) {        // HI_MPI_SYS_Init + AcodecConfig(16k) + AI_Enable
        printf("[my_wake] capture init failed\n");
        return 1;
    }
    if (wake_word_init() != 0) {         // TFLM 模型加载 + microfrontend 初始化
        printf("[my_wake] model init failed\n");
        return 1;
    }
    printf("[my_wake] listening...\n");

    for (;;) {
        if (WakeCaptureRead(s_audio, SAMPLES_PER_READ) != 0) {
            continue;
        }
        bool detected = false;
        for (int step = 0; step < STEPS_PER_READ; step++) {
            if (wake_word_process(s_audio + step * WAKE_WORD_SAMPLES_PER_STEP)) {
                detected = true;
            }
        }
        float score = wake_word_last_probability();
        float level = block_rms_dbfs(s_audio, SAMPLES_PER_READ);

        // 定期状态打印(诊断)
        // ... (参考 ESP 项目 main.c 的 STATUS_LOG_US 逻辑)

        if (detected) {
            printf("[my_wake] *** 唤醒! (score %.3f) ***\n", score);
            wake_word_reset();
        }
    }
    return 0;
}
```

### 6.2 检测判定参数（从 ESP 项目继承）

| 参数 | 值 | 来源 |
|---|---|---|
| 滑动窗口 | 5 slices | microwakeword/test.py |
| 概率 cutoff | 0.98 | ESP 项目当前值，需根据板载麦克风实测调整 |
| 不应期 | 1500ms（25 slices × 30ms） | test.py 忽略 accept 后 25 slices |
| 前端窗口 | 30ms / 10ms step / 40 channels | pymicro-features |
| 特征缩放 | 0.0390625 | microwakeword/data.py |
| 反量化 | output / 255 | microwakeword/inference.py |

---

## 7. 实施步骤（分 4 步，每步可独立验证）

### 步骤 1：采集链路验证（不接推理）
- 新建 wake/ 目录，写 wake_capture.c（16kHz HI_MPI_AI 采集）
- wake_main.cc 先只做：取帧 → 算 RMS dBFS → 打印
- 编译进 rootfs，板子上 `exec /bin/my_wake` 验证 16kHz 采集
- **验证标志**：串口看到 mic dBFS 随声音变化

### 步骤 2：vendor TFLM + microfrontend，编译通过
- 拷 tflm/ 和 microfrontend/ 进 wake/
- 写 BUILD.gn 收集 TFLM 源文件（排除 esp_nn/）
- wake_word.cc 搬进来，替换 3 处 ESP 依赖
- **验证标志**：ninja 链接出 my_wake 二进制，无 undefined symbol

### 步骤 3：模型加载 + 单次推理验证
- wake_word_init() 加载 Luna 模型
- 喂已知 PCM，看 score 是否上升
- **验证标志**：input tensor 规格 3×40 int8=120 bytes 匹配，AllocateTensors 成功

### 步骤 4：端到端实时唤醒
- 主循环：取帧 → 前端 → 推理 → 滑动窗口 → cutoff 判定
- 对麦克风说 "Luna"，看是否触发
- 调 cutoff（ESP 用 0.98，板载麦克风可能要调）
- **验证标志**：说 Luna 触发，背景噪声不误触发

---

## 8. 风险与对策

| 风险 | 影响 | 对策 |
|---|---|---|
| flatbuffers 版本冲突 | schema_generated.h 钉死 25.9.23，树里 25.2.10 | include 顺序保证 TFLM 自带的优先 |
| tensor arena 大小不足 | AllocateTensors 失败 | ESP 项目 60KB，Hi3516 内存充裕，不够就加 |
| C++ 异常/RTTI 符号缺失 | 链接报 `__cxa_*` 未定义 | 用 -fno-rtti -fno-exceptions；musl libstdc++ 支持 |
| HI_MPI_AI 16kHz 帧点数 480 不被接受 | 采集失败 | 不行就用 320（20ms）取 2 帧拼 |
| 板载麦克风频响与 ESP 不同 | cutoff 0.98 不适用 | 步骤 4 实测调整，参考 ESP 项目的 cutoff 历史记录 |
| 8kHz→16kHz 带宽变化对唤醒率影响 | 未知 | 16kHz 更宽（奈奎斯特 8kHz），理论应更好 |

---

## 9. 参考文件索引

### 9.1 ESP 项目（源）

| 文件 | 作用 |
|---|---|
| `streaming_model_deployment/main/main.c` | 主循环，RMS 计算，检测后处理 |
| `streaming_model_deployment/main/wake_word.h` | TFLM 推理接口定义 |
| `streaming_model_deployment/main/wake_word.cc` | TFLM 推理 + 滑动窗口实现 |
| `streaming_model_deployment/main/wake_word_model_data.c` | Luna 模型数据（391KB） |
| `streaming_model_deployment/main/audio_input.c` | ES7210 采集（参考结构，硬件不同） |
| `streaming_model_deployment/components/microfrontend/` | TFLM 音频前端（纯 C，直接搬） |
| `streaming_model_deployment/managed_components/espressif__esp-tflite-micro/` | TFLM 引擎（vendor，排除 esp_nn/） |

### 9.2 Hi3516 项目（参考）

| 文件 | 作用 |
|---|---|
| `device/soc/hisilicon/common/platform/mytest/audio/audio_pcm.c` | my_audio 实现（参考 HI_MPI_AI 用法） |
| `device/soc/hisilicon/common/platform/mytest/audio/audio_pcm.h` | 录音参数定义 |
| `device/soc/hisilicon/common/platform/mytest/audio/BUILD.gn` | 静态链接 mpp 库的参考写法 |
| `device/soc/hisilicon/hi3516dv300/sdk_liteos/include/hi_comm_aio.h` | AIO 属性/采样率枚举 |
| `device/soc/hisilicon/hi3516dv300/sdk_liteos/include/acodec.h` | ACodec ioctl 定义 |
| `build/lite/config/component/lite_component.gni` | executable 模板（C++ 支持） |

---

## 10. 验收标准

| 项 | 标准 |
|---|---|
| 编译 | `hb build` 成功，my_wake 进 rootfs/bin |
| 采集 | `exec /bin/my_wake` 打印 mic dBFS 随声音变化 |
| 模型 | wake_word_init 打印 "model ready"，arena used < 60KB |
| 唤醒 | 对麦克风说 "Luna" 触发 `*** 唤醒! ***` 打印 |
| 误触发 | 安静环境 60 秒内不触发 |
| 响应延迟 | 说完唤醒词到触发 < 1 秒 |

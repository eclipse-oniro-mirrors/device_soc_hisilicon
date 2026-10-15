# pca9685_sdk — PCA9685 舵机/Otto 双足动作 用户态 SDK

16 路 PWM 舵机驱动（I2C，PCA9685）+ Otto 双足机器人 40+ 个协同动作。
纯用户态：业务进程直接调用，基于 HDF 用户态 I2C HDI（与内核版同名同 API）。

## 两层 API

### 1. pca9685.h — 基础层（单舵机/芯片控制）

| 函数 | 作用 |
|---|---|
| `PcaInit()` | 一键初始化（幂等，可重复调） |
| `PcaInitEx(bus, addr)` | 指定总线/地址（改过 A0~A5 跳线时用） |
| `PcaScanBus(bus)` | 扫描总线上有应答的器件，返回个数 |
| `PcaServoAngle(ch, deg)` | 单舵机 0~180°（自动裁剪） |
| `PcaServoUs(ch, us)` | 直接给脉宽 500~2500us |
| `PcaAllChannelsOff()` | 全通道停发脉冲（舵机松弛失力） |
| `PcaGetStatus(...)` | 查本进程就绪态 |
| `PcaEnsureReady(bus, addr, force)` | 底层初始化（业务一般不用） |

### 2. otto_pca.h — 动作层（Otto 六舵机协同，CH0~CH5 = ll rl lf rf lh rh）

| 函数 | 作用 |
|---|---|
| `OttoPcaWalk()` / `WalkEx(steps,ms,dir,arm)` | 前走（默认 4 步 1000ms） |
| `OttoPcaTurn()` / `TurnEx(...)` | 转弯（dir=1 左 / -1 右） |
| `OttoPcaJump / Bend / Shakeleg / Updown / Swing / Tiptoe / Jitter / Ascend` | 律动类 |
| `OttoPcaCrusaito / Flapping / Moonwalk / Whirlwind / Windmill / Takeoff` | 舞步类 |
| `OttoPcaHandsup / Handsdown / Handwave / Greeting / Shy / Fitness` | 手势类 |
| `OttoPcaMagic / Calisthenics / Showcase` | 长动作串（28s/32s/2~3min） |
| `OttoPcaHome()` | 归位 [90 90 90 90 45 135] |
| `OttoPcaSit()` | 坐下（不归位） |
| `OttoPcaRun("walk", par, npar)` | 名字驱动通用入口（参数不满自动补默认） |
| `OttoPcaPost("walk", par, npar)` | 异步投后台任务（立即返回，常驻进程用） |
| `OttoPcaStop()` | 请求中止当前动作 → 自动归位 |
| `OttoPcaList/Map/Pose()` | 动作表 / 通道映射 / 姿态对照打印 |
| `OttoPcaSetTrim(limb, val)` | 运行期微调 -50~50（不落盘） |

全部便捷动作**阻塞执行**，结束自动归位（sit/handsup/showcase 除外）。
返回 0=完成，-1=被中止/未就绪，-3=动作名未知。

## 文件清单

| 文件 | 用途 |
|---|---|
| pca9685.h / pca9685_core.c | 基础层源码 |
| otto_pca.h / otto_pca.c | 动作层源码（平台垫片 pca_port.h） |
| pca_demo.c | 板端命令行测试程序源码 |
| libpca9685.a / libpca9685.so | 预编译库（静态/动态） |
| pca9685_core.o / otto_pca.o / pca_demo.o | 预编译目标文件 |
| pca_demo | 预编译板端程序（push 到 /bin 后 exec 调用） |
| build_pca9685.sh / build_pca_demo.sh | 全套交叉编译脚本（出 .o/.a/.so/程序） |
| BUILD.gn / bundle.json | 源码树集成文件（见《量产集成说明.md》） |

## 快速接入（业务代码）

```c
#include "pca9685.h"
#include "otto_pca.h"

PcaInit();                        /* 幂等，失败返回 HDF_FAILURE */

PcaServoAngle(0, 90);             /* CH0 单舵机 90° */

OttoPcaWalk();                    /* 默认参数走 4 步（阻塞，完成自动归位） */
OttoPcaWalkEx(2, 800, -1, 0);     /* 后退 2 步（dir=-1） */
OttoPcaRun("crusaito", NULL, 0);  /* 名字驱动，全部默认参数 */

/* 常驻进程可改异步：投给后台任务立即返回 */
OttoPcaStart();                   /* 建后台动作任务（Post 会自动调） */
OttoPcaPost("jump", NULL, 0);
...
OttoPcaStop();                    /* 中止当前动作，自动归位 */
PcaAllChannelsOff();              /* 全部松弛 */
```

板端不用写代码，直接命令行验收（每次 exec 都是新进程）：

```
exec /bin/pca_demo scan                 扫描总线（期望 0x40 ACK）
exec /bin/pca_demo init                 初始化芯片
exec /bin/pca_demo angle 0 90           CH0 到 90°
exec /bin/pca_demo run walk 2 800 1 0   走 2 步
exec /bin/pca_demo list                 动作全表
exec /bin/pca_demo off                  全部松弛
```

## 接线（必须共地）

```
PCA9685 双排 16 路 CH0~CH15 排针：舵机棕/黑→GND，红→V+，橙/黄/白→PWM 针
PCA9685 VCC → 板子 3.3V      GND → 板子 GND
SDA/SCL   → 板上暴露的 I2C6 接口（Y18=I2C6_SCL，AA18=I2C6_SDA，
             开机已复用好，不用自己切）
V+        → 舵机独立 5V 电源正（建议 2A 以上）
⚠️ 外部电源 GND 必须与板子 GND 共地，否则 I2C 和信号都不通
```

I2C6 与板载音频功放 TFA9879 共用（0x6D 与 PCA9685 默认 0x40 不冲突，
HDF 传输自动串行化，互不干扰）。

## 两种链接方式（.a 静态 / .so 动态）——按场景选

| 场景 | 选择 | 理由 |
|---|---|---|
| 只有一个程序用 SDK（**默认推荐**） | `.a` 静态链 | 产物单文件、无部署依赖 |
| 镜像里多个进程都要调 SDK | `.so` 动态链 | 共享一份代码；升级只换 .so |

### 静态链

```bash
clang --target=arm-liteos-eabi -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 \
      --sysroot=$SYSROOT <HDF头7个-I> 你的代码.c ./libpca9685.a \
      -L$OUT/usr/lib -lhdf_platform -o 你的程序
```

⚠️ 显式给 `./libpca9685.a` 路径，**不要**写 `-L. -lpca9685`——同目录有 .so 时
链接器会优先动态链，板上没有该 .so 会 exec 失败（实测踩过）。

### 动态链

```bash
clang ... 你的代码.c -L. -lpca9685 -L$OUT/usr/lib -lhdf_platform -o 你的程序
```

部署时必须把 `libpca9685.so` 放板上 **`/usr/lib/`**（板上 ldso 不认
LD_LIBRARY_PATH）。

HDF 用户态头（编译时 -I，7 个）：

```
drivers/hdf_core/framework/include/{core,osal,platform,utils}
drivers/hdf_core/interfaces/inner_api/{core,osal/shared,osal/uhdf,utils}
```

链接依赖 `libhdf_platform.so`（板上 /usr/lib 已有，镜像自带）。
build_pca9685.sh / build_pca_demo.sh 里有可直接照抄的完整旗标。

## 注意事项（板上实测坑）

1. SDK 状态**不跨进程共享**：每次 `exec` 都是新进程，各进程先 `PcaInit`；
   `pca_demo stop` 无法中止另一个进程正在跑的动作（用 API 的 OttoPcaStop 才行）
2. 串口调用必须 `exec /bin/pca_demo ...`（LiteOS shell 超长命令名静默丢弃的坑）
3. 动作执行期间不要并行发 angle/pose 写通道命令，会与编排抢拍
4. 舵机乱抖/不动：先查共地，再查 V+ 供电（2A 以上），最后 `scan` 确认 0x40 ACK
5. 想固化微调角度：改 otto_pca.h 的 `OTTO_LIMB_TRIM_TAB` 重编（`pca_demo trim`
   只对当前进程生效）；装反某肢体改 `OTTO_LIMB_REV_TAB` 置 1，不用改动作表
6. 与内核版（mytest/pca9685）差异仅三处平台替换（delay/tick/线程）+ PWM 舵机
   无位置反馈/无扭矩寄存器；上层 API 名字与语义完全一致

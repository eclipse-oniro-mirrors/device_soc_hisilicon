# libyaffs2.a provenance

| 项 | 值 |
|---|---|
| 来源工程 | `260903_fbb/hs-fbb`（fbb SDK 工作区） |
| 原始路径 | `src/kernel/liteos/LiteOS/build/lib_a/diting-community/libyaffs2.a` |
| 原始 sha256 | `790d4973c5cb9c62b677716e286f0258c0fa7918f168dc8fb8d4b3f8d319926c` |
| 变体选择 | `diting-community`（fbb 主系统配置；5 份变体符号集等价 349 T，唯 `diting-community-xts` 缺 `LOS_YaffsSetMemPool`） |
| 本地补丁后 sha256 | `485e51c15ad8a8e0c9207d459de0b7c78e88a1aa5769bcd4f1a97add7515e2ab` |

## 当前链接形态（D 路线硬浮点，2026-09-22 起）

**构建直接链接本目录 `libyaffs2.orig.a`（fbb 原版，零补丁）。** 本工程切换到
`-march=rv32imfdcxlinxma_xlinxmb_xlinxmc_xlinxmd -mabi=ilp32d` 后，自编对象 e_flags
同为 `0x405`（含 BiSheng 对 xlinx* 构建自动标记的 vendor 位），与 fbb 归档逐位一致，
lld 直接放行（链接实验见根目录 `HI3322_HARDFLOAT_IMPACT_ANALYSIS.md` §4.3-B）。
补丁版 `libyaffs2.a` 与 `tools/patch_eflags.py` 保留在库，作为回退软浮点 ABI
（`rv32imc/ilp32`，e_flags `0x1`）时的可复现工具，常规构建不再使用。

## 补丁说明（tools/patch_eflags.py，软浮点回退路线专用，可复现）

- fbb 构建参数为 `-march=rv32imfdcb0p92xlinxma_xlinxmb_xlinxmc -mabi=ilp32d`，
  归档成员 ELF e_flags = `0x405`；软浮点工程链接 ABI 为 `rv32imc/ilp32`（e_flags = `0x1`）。
  lld 拒绝混链（`cannot link object files with different floating-point ABI`）。
- 全库零浮点指令（指令普查无 `f*.s/f*.d/f*.w`），改写 float-ABI 标志位语义无效。
- 库内 Zcmp/Zcb/Zbb 指令（`c.push/c.pop/c.zext.b/rev8/andn` 等）由 hi3322 核执行——
  fbb 产线固件携带同一归档在同款芯片运行；本工程 `-march=rv32imc` 为其保守子集。
- 脚本逐成员把 ELF32 头 0x24 偏移的 e_flags `0x405 → 0x1`，按原成员顺序重组，
  并校验全局文本符号数（349）不变。`.riscv.attributes` 保持原样（lld 15 不校验）。

## 头文件契约

`../include/` 为 fbb 原版头文件集（`self_src/fs/yaffs2/` 的 .h + `fs/include/mtd_partition.h`
+ `compat/linux/include/linux/mtd/` 三件套，mtd 三件套取自 morpheus fmc 移植的
fbb-verbatim 拷贝）。**注意：不要使用 morpheus 源码路线的补丁版头**（该版为对齐上游 .c
改过 `yaffs_endian.h`/`yaffs_verify.h`）；闭库按 fbb 原版头编译，两侧必须同源。

烤入配置（不可通过重编译调整）：`n_caches=20`、`n_reserved_blocks=5`、`use_nand_ecc=1`、
`no_tags_ecc=0`、`is_yaffs2=1`、无后台 GC 线程、独立内存池模式（`LOS_YaffsSetMemPool`）。
详见根目录 `YAFFS2_BINARY_LIB_PORTING_PLAN.md` §3.4。

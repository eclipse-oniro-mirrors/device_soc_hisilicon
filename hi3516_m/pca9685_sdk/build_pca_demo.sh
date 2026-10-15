#!/bin/bash
# build_pca_demo.sh - 编译板端测试程序 pca_demo(静态链 libpca9685.a)
# 用法: bash build_pca_demo.sh [OH源码根目录]   (默认 /code/3516)
# 前置: 先跑过 build_pca9685.sh(需要本目录 libpca9685.a)
# 产物: pca_demo (push 到板子 /bin/ 后用 'exec /bin/pca_demo ...' 调用)
set -e
cd "$(dirname "$0")"

REPO="${1:-/code/3516}"
OUT="$REPO/out/hispark_taurus/ipcamera_hispark_taurus"
CLANG="$REPO/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang"

FLAGS="--target=arm-liteos-eabi -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -O2 -Wall"
SYS="--sysroot=$OUT/sysroot"
HDF_I="-I$REPO/drivers/hdf_core/framework/include/core \
       -I$REPO/drivers/hdf_core/framework/include/osal \
       -I$REPO/drivers/hdf_core/framework/include/platform \
       -I$REPO/drivers/hdf_core/framework/include/utils \
       -I$REPO/drivers/hdf_core/interfaces/inner_api/core \
       -I$REPO/drivers/hdf_core/interfaces/inner_api/osal/shared \
       -I$REPO/drivers/hdf_core/interfaces/inner_api/osal/uhdf \
       -I$REPO/drivers/hdf_core/interfaces/inner_api/utils"

echo "[cc] pca_demo.c"
"$CLANG" $FLAGS $SYS $HDF_I -c pca_demo.c -o pca_demo.o

# 注意:显式给 .a 路径,避免 -lpca9685 同目录存在 .so 时被优先动态链接
echo "[ld] pca_demo (静态链 libpca9685.a + 动态链 libhdf_platform.so)"
"$CLANG" $FLAGS $SYS -o pca_demo pca_demo.o ./libpca9685.a -L"$OUT/usr/lib" -lhdf_platform

echo "done: $(ls -l pca_demo pca_demo.o | awk '{print $NF, $5}')"

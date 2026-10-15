#!/bin/bash
# build_pca9685.sh - 交叉编译 PCA9685/Otto SDK -> .o/.a/.so
# 用法: bash build_pca9685.sh [OH源码根目录]   (默认 /code/3516)
# 产物: libpca9685.a (静态) + libpca9685.so (动态) + 中间 .o,均在脚本所在目录
set -e
cd "$(dirname "$0")"

REPO="${1:-/code/3516}"
OUT="$REPO/out/hispark_taurus/ipcamera_hispark_taurus"
CLANG="$REPO/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang"
AR="$REPO/prebuilts/clang/ohos/linux-x86_64/llvm/bin/llvm-ar"

FLAGS="--target=arm-liteos-eabi -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -O2 -Wall"
SYS="--sysroot=$OUT/sysroot"
# HDF 用户态头(i2c_if 的用户态实现 i2c_if_u 在 libhdf_platform.so 里)
HDF_I="-I$REPO/drivers/hdf_core/framework/include/core \
       -I$REPO/drivers/hdf_core/framework/include/osal \
       -I$REPO/drivers/hdf_core/framework/include/platform \
       -I$REPO/drivers/hdf_core/framework/include/utils \
       -I$REPO/drivers/hdf_core/interfaces/inner_api/core \
       -I$REPO/drivers/hdf_core/interfaces/inner_api/osal/shared \
       -I$REPO/drivers/hdf_core/interfaces/inner_api/osal/uhdf \
       -I$REPO/drivers/hdf_core/interfaces/inner_api/utils"

SRCS="pca9685_core.c otto_pca.c"
OBJS=""
for s in $SRCS; do
    o="${s%.c}.o"
    echo "[cc] $s"
    "$CLANG" $FLAGS $SYS $HDF_I -c "$s" -o "$o"
    OBJS="$OBJS $o"
done

echo "[ar] libpca9685.a"
"$AR" rcs libpca9685.a $OBJS

echo "[so] libpca9685.so"
"$CLANG" $FLAGS $SYS -shared -o libpca9685.so $OBJS -L"$OUT/usr/lib" -lhdf_platform

echo "done: $(ls -l libpca9685.a libpca9685.so $OBJS | awk '{print $NF, $5}')"

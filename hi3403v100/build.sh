#!/bin/bash
# Copyright (c) 2025 HiSilicon (Shanghai) Technologies Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
 
set -e
OHOS_ROOT_PATH=$1
OHOS_OUTDIR=$2
if [ -n "${OHOS_OUTDIR}" ] && [[ "${OHOS_OUTDIR}" != /* ]]; then
    OHOS_OUTDIR="$(pwd)/${OHOS_OUTDIR}"
fi
OHOS_LITE=$3
COMPILER_DIR=$4
CHIP=$6
 
export KERNEL_VERSION="$5"
 
if [ -z "${OHOS_ROOT_PATH}" ];then
    OHOS_ROOT_PATH=$(pwd)/../../../..
else
    echo "OHOS_ROOT_PATH=${OHOS_ROOT_PATH}"
    if [[ "${OHOS_ROOT_PATH}" != /* ]]; then
        OHOS_ROOT_PATH="$(pwd)/${OHOS_ROOT_PATH}"
    fi
fi
 
export OHOS_ROOT_PATH
export OHOS_OUTDIR
if [ ${COMPILER_DIR} != "" ];then
    export COMPILER_PATH=${COMPILER_DIR}/bin
    export PATH=${COMPILER_PATH}:${PATH}
fi
 
CHIP_PATH=${OHOS_ROOT_PATH}/device/soc/hisilicon/hi3403v100
BATCH_SIGN_KO_SCRIPT=${OHOS_ROOT_PATH}/device/board/hisilicon/hispark_aifly/kernel/batch_sign_ko.sh

# hardware / modules / config 按发布包结构整体拷到 out 下编译，不改动源码树
STAGE_PATH=${OHOS_OUTDIR}/sdk_linux/src_tmp
HARDWARE_TMP_PATH=${STAGE_PATH}/hardware
MODULES_TMP_PATH=${STAGE_PATH}/modules
CONFIG_TMP_PATH=${STAGE_PATH}/config
SDK_OUT_PATH=${STAGE_PATH}/out
SYSROOT_PATH=${OHOS_OUTDIR}/sysroot
ENV_SIZE=0x40000
UBOOT_ENV_4GB_BIN=${CHIP_PATH}/bootloader/uboot_env_4GB.bin
UBOOT_ENV_4GB_TXT=${CHIP_PATH}/bootloader/uboot_env_4GB.txt
export SYSROOT_PATH
 
rm -rf ${STAGE_PATH}
mkdir -p ${STAGE_PATH}
cp -rf ${CHIP_PATH}/hardware ${HARDWARE_TMP_PATH}
cp -rf ${CHIP_PATH}/modules ${MODULES_TMP_PATH}
cp -rf ${CHIP_PATH}/config ${CONFIG_TMP_PATH}

mkdir -p ${SDK_OUT_PATH}/ko
# 加载脚本需在 ko 编译前就位：mpp/cbb/Makefile.linux 编完模块会对 $(REL_KO)/load* 做 chmod
cp -f ${MODULES_TMP_PATH}/ko/load_hi3403v100_ohos ${SDK_OUT_PATH}/ko/ 2>/dev/null || true

# libmpp_*.so 以预编译形态随仓库提供，形态与原厂的 6 个合并库完全一致：
# 仓库里只有 .so，没有 .a，也不在构建期做任何库合并。
# 裁剪后的 5 个库（media/codec/isp/audio/sec）是离线合并一次的产物；
# libmpp_ai.so（SVP ACL）仅服务 mindspore_feature_svp，与 AICPU 算子库
# libsvp_aicpu.so 一同平铺在 hardware/lib 下，不参与合并。
echo "compile ko..."
# 不能写成 "pushd X && make clean && make -j && popd"：set -e 对 && 非末项失败豁免，失败会被吞掉。
pushd "${MODULES_TMP_PATH}"
make clean OHOS_LITE=y LLVM=1 CHIP="${CHIP}" SYSROOT_PATH="${SYSROOT_PATH}"
make -j OHOS_LITE=y LLVM=1 CHIP="${CHIP}" SYSROOT_PATH="${SYSROOT_PATH}"
popd

# 传感器库（libsns_*.so）：相机初始化时由 cameradev ini 按 sensor_lib 路径 dlopen，
# 必须在硬件库之后编出来并放进 hardware/lib。
echo "compile sensor libs..."
for s in hy_s0603 omnivision_os08a20; do
    if [ -d "${HARDWARE_TMP_PATH}/sensor/${s}" ]; then
        echo "  build ${s}"
        ( cd "${HARDWARE_TMP_PATH}/sensor/${s}" && \
          make clean OHOS_LITE=y LLVM=1 CHIP="${CHIP}" SYSROOT_PATH="${SYSROOT_PATH}" > /dev/null 2>&1 || true; \
          make OHOS_LITE=y LLVM=1 CHIP="${CHIP}" SYSROOT_PATH="${SYSROOT_PATH}" )
    fi
done


# batch sign ko file: sign in-place on copied output
chmod +x ${BATCH_SIGN_KO_SCRIPT}
${BATCH_SIGN_KO_SCRIPT} -o ${SDK_OUT_PATH} ${SDK_OUT_PATH}

# verify ko files: 逐个点名校验，缺任何一个都让构建失败。
EXPECTED_KOS="sys_config.ko ot_osal.ko ot_mipi_rx.ko ot_mipi_tx.ko \
ot_irq.ko ot_user_proc.ko gfbg.ko ot_cipher.ko ot_klad.ko ot_otp.ko \
ot_base.ko ot_sys.ko ot_tde.ko ot_hdmi.ko ot_vo.ko"
MISSING_KOS=""
for ko in ${EXPECTED_KOS}; do
    if [ ! -f "${SDK_OUT_PATH}/ko/${ko}" ]; then
        MISSING_KOS="${MISSING_KOS} ${ko}"
    fi
done
if [ -n "${MISSING_KOS}" ]; then
    echo "SDK KO: ${SDK_OUT_PATH}/ko verification failed, missing:${MISSING_KOS}"
    echo "SDK KO: current ko dir content:"
    ls -l ${SDK_OUT_PATH}/ko/ 2>/dev/null || true
    echo "SDK KO: check the make log above for the first failing component"
    exit 1
fi
echo "SDK KO: ${SDK_OUT_PATH}/ko verification success, all ko file(s) found"

# copy lib to out/lib for fs.yml packaging
SDK_LIB_PATH=${HARDWARE_TMP_PATH}/lib

OUT_LIB_PATH=${SDK_OUT_PATH}/lib
mkdir -p ${OUT_LIB_PATH}
find ${SDK_LIB_PATH} -type f -name "*.so" -exec cp -f {} ${OUT_LIB_PATH}/ \;

# libmpp_*.so 是预编译库，直接来自 hardware/lib。
# 保留下来的 legacy 模块名以软链指向合并库，兼容仍在使用这些名字的消费者
# （dlopen 名字、预编译库的 DT_NEEDED）。
for n in libaac_enc libaac_dec libaac_sbr_enc libaac_sbr_dec \
         libaac_comm libvqe_aec libvqe_agc libvqe_anr libvqe_eq \
         libss_dnvqe libss_upvqe libss_voice_engine; do
    ln -sf libmpp_audio.so "${OUT_LIB_PATH}/${n}.so"
done
for n in libss_mpi libot_irq libss_hdmi libss_tde; do
    ln -sf libmpp_media.so "${OUT_LIB_PATH}/${n}.so"
done
for n in libot_sample_ist libot_sensor_i2c libot_sensor_spi; do
    ln -sf libmpp_isp.so "${OUT_LIB_PATH}/${n}.so"
done
for n in libot_aio libot_ai libot_ao libot_aenc libot_adec libot_acodec; do
    ln -sf libmpp_audio.so "${OUT_LIB_PATH}/${n}.so"
done

# copy uboot file
mkenvimage -s $ENV_SIZE -o $UBOOT_ENV_4GB_BIN $UBOOT_ENV_4GB_TXT
cp -rf ${CHIP_PATH}/bootloader/* ${OHOS_OUTDIR}

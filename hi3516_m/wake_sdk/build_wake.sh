#!/bin/bash
# build_wake.sh - 独立交叉编译 my_wake(语音唤醒+实时对话+FC),不依赖源码树 GN
# 用法: bash build_wake.sh [OH源码根目录]   (默认 /code/3516)
# 前置: Hi3516 树编译过一次(out 里有 sysroot + mpp/lib/*.a + libs/libmbedtls.a)
# 产物: my_wake(板端程序, 烧 rootfs 后在 /bin/my_wake) + build/ 全部中间 .o
# 源码清单与包内 BUILD.gn 完全一致(vendored tflm/microfrontend 全量编译)。
set -e
cd "$(dirname "$0")"

REPO="${1:-/code/3516}"
OUT="$REPO/out/hispark_taurus/ipcamera_hispark_taurus"
CLANG="$REPO/prebuilts/clang/ohos/linux-x86_64/llvm/bin/clang"
MPP="$REPO/device/soc/hisilicon/hi3516dv300/sdk_liteos/mpp/lib"

BASE="--target=arm-liteos-eabi -mcpu=cortex-a7 -mfloat-abi=softfp -mfpu=neon-vfpv4 -O2"
SYS="--sysroot=$OUT/sysroot"
WARN="-Wno-unused-parameter -Wno-sign-compare -Wno-missing-field-initializers -Wno-double-promotion -Wno-unused-function -Wno-type-limits -Wno-strict-aliasing"
CFLAGS="$BASE -DTF_LITE_STATIC_MEMORY -DTF_LITE_DISABLE_X86_NEON -ffunction-sections -fdata-sections -fPIC $WARN"
CXXFLAGS="$CFLAGS -fno-rtti -fno-exceptions -fno-threadsafe-statics -std=c++11"

SDK_INC="$REPO/device/soc/hisilicon/hi3516dv300/sdk_liteos/include"
INC="-I$SDK_INC -I$REPO/third_party/bounds_checking_function/include -I$REPO/third_party/mbedtls/include -I. -Itflm -Itflm/third_party/flatbuffers/include -Itflm/third_party/gemmlowp -Itflm/third_party/ruy -Itflm/third_party/kissfft -Imicrofrontend -Imicrofrontend/kissfft"

compile_one() {
    s="$1"
    o="build/${s//\//_}.o"
    case "$s" in
        *.cc) "$CLANG" $CXXFLAGS $SYS $INC -c "$s" -o "$o" ;;
        *)    "$CLANG" $CFLAGS  $SYS $INC -c "$s" -o "$o" ;;
    esac
}
export -f compile_one
export CLANG CFLAGS CXXFLAGS SYS INC

cat > build_srcs.txt <<'SRCS'
tflm/tensorflow/lite/micro/kernels/activations.cc tflm/tensorflow/lite/micro/kernels/activations_common.cc tflm/tensorflow/lite/micro/kernels/add.cc
tflm/tensorflow/lite/micro/kernels/add_common.cc tflm/tensorflow/lite/micro/kernels/add_n.cc tflm/tensorflow/lite/micro/kernels/arg_min_max.cc
tflm/tensorflow/lite/micro/kernels/assign_variable.cc tflm/tensorflow/lite/micro/kernels/batch_matmul.cc tflm/tensorflow/lite/micro/kernels/batch_matmul_common.cc
tflm/tensorflow/lite/micro/kernels/batch_to_space_nd.cc tflm/tensorflow/lite/micro/kernels/broadcast_args.cc tflm/tensorflow/lite/micro/kernels/broadcast_to.cc
tflm/tensorflow/lite/micro/kernels/call_once.cc tflm/tensorflow/lite/micro/kernels/cast.cc tflm/tensorflow/lite/micro/kernels/ceil.cc
tflm/tensorflow/lite/micro/kernels/circular_buffer.cc tflm/tensorflow/lite/micro/kernels/circular_buffer_common.cc tflm/tensorflow/lite/micro/kernels/comparisons.cc
tflm/tensorflow/lite/micro/kernels/concatenation.cc tflm/tensorflow/lite/micro/kernels/conv.cc tflm/tensorflow/lite/micro/kernels/conv_common.cc
tflm/tensorflow/lite/micro/kernels/cumsum.cc tflm/tensorflow/lite/micro/kernels/decode.cc tflm/tensorflow/lite/micro/kernels/decode_state.cc
tflm/tensorflow/lite/micro/kernels/decode_state_huffman.cc tflm/tensorflow/lite/micro/kernels/decode_state_lut.cc tflm/tensorflow/lite/micro/kernels/decode_state_prune.cc
tflm/tensorflow/lite/micro/kernels/decompress.cc tflm/tensorflow/lite/micro/kernels/decompress_common.cc tflm/tensorflow/lite/micro/kernels/depth_to_space.cc
tflm/tensorflow/lite/micro/kernels/depthwise_conv.cc tflm/tensorflow/lite/micro/kernels/depthwise_conv_common.cc tflm/tensorflow/lite/micro/kernels/dequantize.cc
tflm/tensorflow/lite/micro/kernels/dequantize_common.cc tflm/tensorflow/lite/micro/kernels/detection_postprocess.cc tflm/tensorflow/lite/micro/kernels/div.cc
tflm/tensorflow/lite/micro/kernels/dynamic_update_slice.cc tflm/tensorflow/lite/micro/kernels/elementwise.cc tflm/tensorflow/lite/micro/kernels/elu.cc
tflm/tensorflow/lite/micro/kernels/embedding_lookup.cc tflm/tensorflow/lite/micro/kernels/exp.cc tflm/tensorflow/lite/micro/kernels/expand_dims.cc
tflm/tensorflow/lite/micro/kernels/fill.cc tflm/tensorflow/lite/micro/kernels/floor.cc tflm/tensorflow/lite/micro/kernels/floor_div.cc
tflm/tensorflow/lite/micro/kernels/floor_mod.cc tflm/tensorflow/lite/micro/kernels/fully_connected.cc tflm/tensorflow/lite/micro/kernels/fully_connected_common.cc
tflm/tensorflow/lite/micro/kernels/gather.cc tflm/tensorflow/lite/micro/kernels/gather_nd.cc tflm/tensorflow/lite/micro/kernels/hard_swish.cc
tflm/tensorflow/lite/micro/kernels/hard_swish_common.cc tflm/tensorflow/lite/micro/kernels/if.cc tflm/tensorflow/lite/micro/kernels/kernel_util.cc
tflm/tensorflow/lite/micro/kernels/l2_pool_2d.cc tflm/tensorflow/lite/micro/kernels/l2norm.cc tflm/tensorflow/lite/micro/kernels/leaky_relu.cc
tflm/tensorflow/lite/micro/kernels/leaky_relu_common.cc tflm/tensorflow/lite/micro/kernels/log_softmax.cc tflm/tensorflow/lite/micro/kernels/logical.cc
tflm/tensorflow/lite/micro/kernels/logical_common.cc tflm/tensorflow/lite/micro/kernels/logistic.cc tflm/tensorflow/lite/micro/kernels/logistic_common.cc
tflm/tensorflow/lite/micro/kernels/lstm_eval.cc tflm/tensorflow/lite/micro/kernels/lstm_eval_common.cc tflm/tensorflow/lite/micro/kernels/maximum_minimum.cc
tflm/tensorflow/lite/micro/kernels/micro_tensor_utils.cc tflm/tensorflow/lite/micro/kernels/mirror_pad.cc tflm/tensorflow/lite/micro/kernels/mul.cc
tflm/tensorflow/lite/micro/kernels/mul_common.cc tflm/tensorflow/lite/micro/kernels/neg.cc tflm/tensorflow/lite/micro/kernels/pack.cc
tflm/tensorflow/lite/micro/kernels/pad.cc tflm/tensorflow/lite/micro/kernels/pad_common.cc tflm/tensorflow/lite/micro/kernels/pooling.cc
tflm/tensorflow/lite/micro/kernels/pooling_common.cc tflm/tensorflow/lite/micro/kernels/prelu.cc tflm/tensorflow/lite/micro/kernels/prelu_common.cc
tflm/tensorflow/lite/micro/kernels/quantize.cc tflm/tensorflow/lite/micro/kernels/quantize_common.cc tflm/tensorflow/lite/micro/kernels/read_variable.cc
tflm/tensorflow/lite/micro/kernels/reduce.cc tflm/tensorflow/lite/micro/kernels/reduce_common.cc tflm/tensorflow/lite/micro/kernels/reshape.cc
tflm/tensorflow/lite/micro/kernels/reshape_common.cc tflm/tensorflow/lite/micro/kernels/resize_bilinear.cc tflm/tensorflow/lite/micro/kernels/resize_nearest_neighbor.cc
tflm/tensorflow/lite/micro/kernels/reverse.cc tflm/tensorflow/lite/micro/kernels/round.cc tflm/tensorflow/lite/micro/kernels/select.cc
tflm/tensorflow/lite/micro/kernels/shape.cc tflm/tensorflow/lite/micro/kernels/slice.cc tflm/tensorflow/lite/micro/kernels/softmax.cc
tflm/tensorflow/lite/micro/kernels/softmax_common.cc tflm/tensorflow/lite/micro/kernels/space_to_batch_nd.cc tflm/tensorflow/lite/micro/kernels/space_to_depth.cc
tflm/tensorflow/lite/micro/kernels/split.cc tflm/tensorflow/lite/micro/kernels/split_v.cc tflm/tensorflow/lite/micro/kernels/squared_difference.cc
tflm/tensorflow/lite/micro/kernels/squeeze.cc tflm/tensorflow/lite/micro/kernels/strided_slice.cc tflm/tensorflow/lite/micro/kernels/strided_slice_common.cc
tflm/tensorflow/lite/micro/kernels/sub.cc tflm/tensorflow/lite/micro/kernels/sub_common.cc tflm/tensorflow/lite/micro/kernels/svdf.cc
tflm/tensorflow/lite/micro/kernels/svdf_common.cc tflm/tensorflow/lite/micro/kernels/tanh.cc tflm/tensorflow/lite/micro/kernels/transpose.cc
tflm/tensorflow/lite/micro/kernels/transpose_common.cc tflm/tensorflow/lite/micro/kernels/transpose_conv.cc tflm/tensorflow/lite/micro/kernels/unidirectional_sequence_lstm.cc
tflm/tensorflow/lite/micro/kernels/unpack.cc tflm/tensorflow/lite/micro/kernels/var_handle.cc tflm/tensorflow/lite/micro/kernels/while.cc
tflm/tensorflow/lite/micro/kernels/zeros_like.cc tflm/signal/micro/kernels/delay.cc tflm/signal/micro/kernels/energy.cc
tflm/signal/micro/kernels/fft_auto_scale_common.cc tflm/signal/micro/kernels/fft_auto_scale_kernel.cc tflm/signal/micro/kernels/filter_bank.cc
tflm/signal/micro/kernels/filter_bank_log.cc tflm/signal/micro/kernels/filter_bank_spectral_subtraction.cc tflm/signal/micro/kernels/filter_bank_square_root.cc
tflm/signal/micro/kernels/filter_bank_square_root_common.cc tflm/signal/micro/kernels/framer.cc tflm/signal/micro/kernels/irfft.cc
tflm/signal/micro/kernels/overlap_add.cc tflm/signal/micro/kernels/pcan.cc tflm/signal/micro/kernels/rfft.cc
tflm/signal/micro/kernels/stacker.cc tflm/signal/micro/kernels/window.cc tflm/signal/src/circular_buffer.cc
tflm/signal/src/energy.cc tflm/signal/src/fft_auto_scale.cc tflm/signal/src/filter_bank.cc
tflm/signal/src/filter_bank_log.cc tflm/signal/src/filter_bank_spectral_subtraction.cc tflm/signal/src/filter_bank_square_root.cc
tflm/signal/src/irfft_float.cc tflm/signal/src/irfft_int16.cc tflm/signal/src/irfft_int32.cc
tflm/signal/src/kiss_fft_wrappers/kiss_fft_float.cc tflm/signal/src/kiss_fft_wrappers/kiss_fft_int16.cc tflm/signal/src/kiss_fft_wrappers/kiss_fft_int32.cc
tflm/signal/src/log.cc tflm/signal/src/max_abs.cc tflm/signal/src/msb_32.cc
tflm/signal/src/msb_64.cc tflm/signal/src/overlap_add.cc tflm/signal/src/pcan_argc_fixed.cc
tflm/signal/src/rfft_float.cc tflm/signal/src/rfft_int16.cc tflm/signal/src/rfft_int32.cc
tflm/signal/src/square_root_32.cc tflm/signal/src/square_root_64.cc tflm/signal/src/window.cc
wake_main.cc wake_capture.c wake_playback.c
wake_ws.c wake_base64.c wake_dialog.c
agent_bridge.c wake_word.cc wake_word_model_data.c
llm_client.c tflm/tensorflow/lite/micro/debug_log.cc tflm/tensorflow/lite/micro/flatbuffer_utils.cc
tflm/tensorflow/lite/micro/memory_helpers.cc tflm/tensorflow/lite/micro/micro_allocation_info.cc tflm/tensorflow/lite/micro/micro_allocator.cc
tflm/tensorflow/lite/micro/micro_context.cc tflm/tensorflow/lite/micro/micro_interpreter.cc tflm/tensorflow/lite/micro/micro_interpreter_context.cc
tflm/tensorflow/lite/micro/micro_interpreter_graph.cc tflm/tensorflow/lite/micro/micro_log.cc tflm/tensorflow/lite/micro/micro_op_resolver.cc
tflm/tensorflow/lite/micro/micro_profiler.cc tflm/tensorflow/lite/micro/micro_resource_variable.cc tflm/tensorflow/lite/micro/micro_time.cc
tflm/tensorflow/lite/micro/micro_utils.cc tflm/tensorflow/lite/micro/recording_micro_allocator.cc tflm/tensorflow/lite/micro/system_setup.cc
tflm/tensorflow/lite/micro/arena_allocator/non_persistent_arena_buffer_allocator.cc tflm/tensorflow/lite/micro/arena_allocator/persistent_arena_buffer_allocator.cc tflm/tensorflow/lite/micro/arena_allocator/recording_single_arena_buffer_allocator.cc
tflm/tensorflow/lite/micro/arena_allocator/single_arena_buffer_allocator.cc tflm/tensorflow/lite/micro/memory_planner/greedy_memory_planner.cc tflm/tensorflow/lite/micro/memory_planner/linear_memory_planner.cc
tflm/tensorflow/lite/micro/memory_planner/non_persistent_buffer_planner_shim.cc tflm/tensorflow/lite/micro/tflite_bridge/flatbuffer_conversions_bridge.cc tflm/tensorflow/lite/micro/tflite_bridge/micro_error_reporter.cc
tflm/tensorflow/lite/kernels/kernel_util.cc tflm/tensorflow/lite/kernels/internal/common.cc tflm/tensorflow/lite/kernels/internal/quantization_util.cc
tflm/tensorflow/lite/kernels/internal/reference/portable_tensor_utils.cc tflm/tensorflow/lite/kernels/internal/reference/comparisons.cc tflm/tensorflow/lite/kernels/internal/portable_tensor_utils.cc
tflm/tensorflow/lite/kernels/internal/tensor_utils.cc tflm/tensorflow/lite/kernels/internal/tensor_ctypes.cc tflm/tensorflow/lite/core/c/common.cc
tflm/tensorflow/lite/core/api/flatbuffer_conversions.cc tflm/tensorflow/lite/core/api/tensor_utils.cc tflm/tensorflow/compiler/mlir/lite/core/api/error_reporter.cc
tflm/tensorflow/compiler/mlir/lite/schema/schema_utils.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/fft.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/fft_util.cc
microfrontend/tensorflow/lite/experimental/microfrontend/lib/filterbank.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/filterbank_util.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/frontend.cc
microfrontend/tensorflow/lite/experimental/microfrontend/lib/frontend_util.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/kiss_fft_int16.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/log_lut.cc
microfrontend/tensorflow/lite/experimental/microfrontend/lib/log_scale.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/log_scale_util.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/noise_reduction.cc
microfrontend/tensorflow/lite/experimental/microfrontend/lib/noise_reduction_util.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/pcan_gain_control.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/pcan_gain_control_util.cc
microfrontend/tensorflow/lite/experimental/microfrontend/lib/window.cc microfrontend/tensorflow/lite/experimental/microfrontend/lib/window_util.cc
SRCS

echo "[1/3] compile $(wc -w < build_srcs.txt) sources (parallel, $(nproc) jobs)"
mkdir -p build
xargs -P "$(nproc)" -n 1 bash -c 'compile_one "$@"' _ < build_srcs.txt

echo "[2/3] link my_wake (静态链 mpp 全家桶 + libwificlient.a + libmbedtls.a; libc++/libc 动态)"
OBJS=""
for s in $(cat build_srcs.txt); do OBJS="$OBJS build/${s//\//_}.o"; done
"$CLANG" $BASE $SYS -o my_wake $OBJS \
    -Wl,--gc-sections \
    -L"$OUT/usr/lib" \
    -Wl,--start-group \
    "$MPP/libmpi.a" "$MPP/libdnvqe.a" "$MPP/libupvqe.a" "$MPP/libVoiceEngine.a" \
    "$MPP/libhive_RES.a" "$MPP/libhive_record.a" "$MPP/libhive_HPF.a" \
    "$MPP/libhive_ANR.a" "$MPP/libhive_AEC.a" "$MPP/libhive_AGC.a" \
    "$MPP/libhive_EQ.a" "$MPP/libaaccomm.a" "$MPP/libsecurec.a" \
    -Wl,--end-group \
    ./libwificlient.a \
    "$OUT/libs/libmbedtls.a" \
    -lm -ldl -lc++

echo "[3/3] strip + done"
"$REPO/prebuilts/clang/ohos/linux-x86_64/llvm/bin/llvm-strip" my_wake
ls -l my_wake
rm -f build_srcs.txt

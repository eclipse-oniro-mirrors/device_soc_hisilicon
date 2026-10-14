# TensorFlow Lite Micro "microfrontend" feature generator.
#
# esp-tflite-micro >= 1.3.x no longer ships tensorflow/lite/experimental/microfrontend,
# but microWakeWord's spectrograms are produced by exactly this code (via the
# pymicro-features package used during training), so it is vendored here to keep
# the on-device features bit-identical to the training features.
#
# Source: pymicro-features 2.0.2 sdist (Apache-2.0 / BSD-3-Clause for kissfft).

set(lib_dir "tensorflow/lite/experimental/microfrontend/lib")

# NOTE: kissfft/kiss_fft.cc and kissfft/tools/kiss_fftr.cc are *textually* included
# by kiss_fft_int16.cc (inside a namespace) and must not be compiled separately.
idf_component_register(
    SRCS
        "${lib_dir}/fft.cc"
        "${lib_dir}/fft_util.cc"
        "${lib_dir}/filterbank.cc"
        "${lib_dir}/filterbank_util.cc"
        "${lib_dir}/frontend.cc"
        "${lib_dir}/frontend_util.cc"
        "${lib_dir}/kiss_fft_int16.cc"
        "${lib_dir}/log_lut.cc"
        "${lib_dir}/log_scale.cc"
        "${lib_dir}/log_scale_util.cc"
        "${lib_dir}/noise_reduction.cc"
        "${lib_dir}/noise_reduction_util.cc"
        "${lib_dir}/pcan_gain_control.cc"
        "${lib_dir}/pcan_gain_control_util.cc"
        "${lib_dir}/window.cc"
        "${lib_dir}/window_util.cc"
    INCLUDE_DIRS "."
)

# kiss_fft_int16.cc does #include "kiss_fft.cc" / #include "tools/kiss_fftr.cc"
target_include_directories(${COMPONENT_LIB} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/kissfft")

target_compile_options(${COMPONENT_LIB} PRIVATE
    -O2
    -Wno-unused-function
    -Wno-sign-compare
    -Wno-missing-field-initializers
)

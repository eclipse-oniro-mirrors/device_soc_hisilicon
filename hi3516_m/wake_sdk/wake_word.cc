// microWakeWord streaming detector.
//
// The feature pipeline mirrors microWakeWord's training pipeline exactly:
//
//   16 kHz int16 -> TFLM microfrontend (30 ms window / 10 ms step, 40 channels,
//   125-7500 Hz, noise reduction + PCAN, log scale) -> uint16 features
//   -> float via *0.0390625 -> int8 via the model's own input quantisation
//
// The frontend settings below are copied from pymicro-features (src/micro_features.cpp),
// which is what microwakeword/audio/audio_utils.py uses to build training spectrograms.
// The scaling constant matches microwakeword/data.py and microwakeword/inference.py.

#include "wake_word.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>

extern "C" {
#include "tensorflow/lite/experimental/microfrontend/lib/frontend.h"
#include "tensorflow/lite/experimental/microfrontend/lib/frontend_util.h"
}

#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_resource_variable.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "wake_word_model_data.h"

#define TAG "wake_word"

namespace {

/* 替换 esp_timer_get_time():返回微秒,用 gettimeofday 实现(perf 统计用,非功能必需) */
static int64_t now_us(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (int64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

// ------------------------------------------------------------ frontend ----
constexpr int kFeatureChannels = 40;
constexpr int kWindowSizeMs = 30;
constexpr int kWindowStepMs = WAKE_WORD_STEP_MS;

// microwakeword/data.py: uint16 spectrograms are scaled by this before the model.
constexpr float kFeatureScale = 0.0390625f;

// --------------------------------------------------------------- model ----
// The streaming model consumes three 10 ms feature slices per invocation, so one
// inference covers 30 ms of new audio.
constexpr int kFeatureSlicesPerInvoke = 3;

// VAR_HANDLE / READ_VARIABLE / ASSIGN_VARIABLE pairs holding the streaming state.
constexpr int kResourceVariableCount = 8;

constexpr int kOpCount = 13;
constexpr size_t kTensorArenaSize = 60 * 1024;

// ----------------------------------------------------------- detection ----
// microwakeword/test.py evaluates with a 5-slice moving average, which is what the
// cutoffs in tflite_streaming_roc.txt refer to.
constexpr int kSlidingWindow = 5;

// Wake word is "Luna" (was "Oliver"; switched because Oliver's /vər/ rhotic ending
// and its two unstressed central vowels are hard for non-native speakers to hit
// consistently).
//
// tflite_streaming_roc.txt for Luna: cutoff 0.82 -> 10% false rejects, 0.750 false
// accepts/hour.
//
// Cutoff history on this board: 0.80 and 0.88 both let near-miss words through
// ("tuna", "lunar", "moon", "露娜", "Tina"), and 0.95 still fired on "luba" and
// "Tina". 0.98 is the current setting under test.
//
// "luba" (/luːbə/) differs from "Luna" (/luːnə/) by a single medial consonant, so
// triggering on it means the model is keying on the /luː-/ onset and weak final
// vowel rather than the consonant that distinguishes them. Raising the cutoff does
// not teach it that distinction -- it only trims whatever scores below the new line.
// If 0.98 still admits these words, thresholding is exhausted and the fix is hard
// negatives in training.
//
// Cost side: tflite_streaming_roc.txt lists 18% false rejects at 0.97 and nothing
// between there and 1.00, so 0.98 is at or above 18% offline. The one encouraging
// data point is the final validation checkpoint (train.py step 10000), which hit
// zero ambient false positives at cutoff 0.98 with 92.1% recall -- but that is the
// non-streaming validation set, not this room.
constexpr float kProbabilityCutoff = 0.98f;

// test.py ignores 25 slices after an accept; each slice is 30 ms.
constexpr int64_t kRefractoryUs = 1500 * 1000;

FrontendConfig g_frontend_config;
FrontendState g_frontend_state;

alignas(16) uint8_t g_tensor_arena[kTensorArenaSize];
tflite::MicroInterpreter *g_interpreter = nullptr;
TfLiteTensor *g_input = nullptr;
TfLiteTensor *g_output = nullptr;

float g_input_scale = 1.0f;
int g_input_zero_point = 0;

// Features accumulated since the last invocation.
int8_t g_pending_features[kFeatureSlicesPerInvoke * kFeatureChannels];
int g_pending_slices = 0;

float g_window[kSlidingWindow];
int g_window_count = 0;
int g_window_head = 0;
float g_last_probability = 0.0f;
int64_t g_suppress_until_us = 0;

// Timing, accumulated over every invocation since the last report.
int64_t g_frontend_us_total = 0;
int64_t g_invoke_us_total = 0;
int64_t g_invoke_us_max = 0;
uint32_t g_invoke_count = 0;
uint32_t g_frontend_count = 0;

void FillFrontendConfig(FrontendConfig *config)
{
    config->window.size_ms = kWindowSizeMs;
    config->window.step_size_ms = kWindowStepMs;

    config->filterbank.num_channels = kFeatureChannels;
    config->filterbank.lower_band_limit = 125.0f;
    config->filterbank.upper_band_limit = 7500.0f;

    config->noise_reduction.smoothing_bits = 10;
    config->noise_reduction.even_smoothing = 0.025f;
    config->noise_reduction.odd_smoothing = 0.06f;
    config->noise_reduction.min_signal_remaining = 0.05f;

    config->pcan_gain_control.enable_pcan = 1;
    config->pcan_gain_control.strength = 0.95f;
    config->pcan_gain_control.offset = 80.0f;
    config->pcan_gain_control.gain_bits = 21;

    config->log_scale.enable_log = 1;
    config->log_scale.scale_shift = 6;
}

int8_t QuantizeFeature(uint16_t raw)
{
    const float scaled = static_cast<float>(raw) * kFeatureScale;
    int value = static_cast<int>(lroundf(scaled / g_input_scale)) + g_input_zero_point;
    if (value < -128) {
        value = -128;
    } else if (value > 127) {
        value = 127;
    }
    return static_cast<int8_t>(value);
}

// Runs the model over the three buffered feature slices and returns the moving
// average of the resulting probabilities.
float RunInference()
{
    memcpy(g_input->data.int8, g_pending_features, sizeof(g_pending_features));

    const int64_t t0 = now_us();
    const TfLiteStatus status = g_interpreter->Invoke();
    const int64_t elapsed = now_us() - t0;

    g_invoke_us_total += elapsed;
    if (elapsed > g_invoke_us_max) {
        g_invoke_us_max = elapsed;
    }
    g_invoke_count++;

    if (status != kTfLiteOk) {
        printf("[wake_word] invoke failed\n");
        return 0.0f;
    }

    // microwakeword/inference.py dequantises the uint8 output as value / 255.
    const float probability = static_cast<float>(g_output->data.uint8[0]) / 255.0f;

    g_window[g_window_head] = probability;
    g_window_head = (g_window_head + 1) % kSlidingWindow;
    if (g_window_count < kSlidingWindow) {
        g_window_count++;
    }

    float sum = 0.0f;
    for (int i = 0; i < g_window_count; i++) {
        sum += g_window[i];
    }
    return sum / static_cast<float>(g_window_count);
}

}  // namespace

extern "C" int wake_word_init(void)
{
    FillFrontendConfig(&g_frontend_config);
    if (!FrontendPopulateState(&g_frontend_config, &g_frontend_state, WAKE_WORD_SAMPLE_RATE)) {
        printf("[wake_word] failed to initialise the microfrontend\n");
        return -1;
    }

    const tflite::Model *model = tflite::GetModel(g_wake_word_model);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        printf("[wake_word] model schema %d, expected %d\n", (int)model->version(),
               TFLITE_SCHEMA_VERSION);
        return -1;
    }

    static tflite::MicroMutableOpResolver<kOpCount> resolver;
    // Op set of trained_models/wakeword/.../stream_state_internal_quant.tflite.
    resolver.AddCallOnce();
    resolver.AddVarHandle();
    resolver.AddReadVariable();
    resolver.AddAssignVariable();
    resolver.AddReshape();
    resolver.AddConcatenation();
    resolver.AddStridedSlice();
    resolver.AddSplitV();
    resolver.AddConv2D();
    resolver.AddDepthwiseConv2D();
    resolver.AddFullyConnected();
    resolver.AddLogistic();
    resolver.AddQuantize();

    tflite::MicroAllocator *allocator =
        tflite::MicroAllocator::Create(g_tensor_arena, kTensorArenaSize);
    if (allocator == nullptr) {
        printf("[wake_word] cannot create the tensor allocator\n");
        return -1;
    }
    tflite::MicroResourceVariables *resource_variables =
        tflite::MicroResourceVariables::Create(allocator, kResourceVariableCount);
    if (resource_variables == nullptr) {
        printf("[wake_word] cannot create resource variables\n");
        return -1;
    }

    static tflite::MicroInterpreter interpreter(model, resolver, allocator, resource_variables);
    g_interpreter = &interpreter;

    if (g_interpreter->AllocateTensors() != kTfLiteOk) {
        printf("[wake_word] AllocateTensors failed; increase kTensorArenaSize\n");
        return -1;
    }

    g_input = g_interpreter->input(0);
    g_output = g_interpreter->output(0);

    if (g_input->type != kTfLiteInt8 ||
        g_input->bytes != kFeatureSlicesPerInvoke * kFeatureChannels) {
        printf("[wake_word] unexpected input tensor: type %d, %d bytes\n", (int)g_input->type,
               (int)g_input->bytes);
        return -1;
    }

    g_input_scale = g_input->params.scale;
    g_input_zero_point = g_input->params.zero_point;

    printf("[wake_word] model ready: arena %u/%u bytes, input scale %.8f zp %d\n",
           (unsigned)g_interpreter->arena_used_bytes(), (unsigned)kTensorArenaSize,
           g_input_scale, g_input_zero_point);
    wake_word_reset();
    return 0;
}

extern "C" void wake_word_reset(void)
{
    FrontendReset(&g_frontend_state);
    g_pending_slices = 0;
    g_window_count = 0;
    g_window_head = 0;
    g_last_probability = 0.0f;
    if (g_interpreter != nullptr) {
        g_interpreter->Reset();
    }
}

extern "C" bool wake_word_process(const int16_t *samples)
{
    size_t samples_read = 0;
    const int64_t t0 = now_us();
    FrontendOutput output = FrontendProcessSamples(&g_frontend_state, samples,
                                                   WAKE_WORD_SAMPLES_PER_STEP, &samples_read);
    g_frontend_us_total += now_us() - t0;
    g_frontend_count++;

    if (output.size == 0) {
        return false;  // still filling the first 30 ms window
    }

    int8_t *slot = g_pending_features + g_pending_slices * kFeatureChannels;
    for (int i = 0; i < kFeatureChannels; i++) {
        slot[i] = QuantizeFeature(output.values[i]);
    }
    if (++g_pending_slices < kFeatureSlicesPerInvoke) {
        return false;
    }
    g_pending_slices = 0;

    g_last_probability = RunInference();

    const int64_t now = now_us();
    if (now < g_suppress_until_us) {
        return false;
    }
    if (g_window_count == kSlidingWindow && g_last_probability > kProbabilityCutoff) {
        g_suppress_until_us = now + kRefractoryUs;
        g_window_count = 0;
        g_window_head = 0;
        return true;
    }
    return false;
}

extern "C" float wake_word_last_probability(void)
{
    return g_last_probability;
}

extern "C" void wake_word_get_stats(wake_word_stats_t *stats)
{
    // Arena high-water mark: TFLM allocates everything up front, so this is the
    // real steady-state figure rather than a sample of a fluctuating heap.
    stats->arena_used_bytes = g_interpreter ? g_interpreter->arena_used_bytes() : 0;
    stats->arena_total_bytes = kTensorArenaSize;

    stats->invoke_count = g_invoke_count;
    stats->invoke_us_avg =
        g_invoke_count ? (float)g_invoke_us_total / (float)g_invoke_count : 0.0f;
    stats->invoke_us_max = g_invoke_us_max;

    // One invoke covers kFeatureSlicesPerInvoke frontend steps, so normalise the
    // frontend cost onto the same per-invoke basis to make the two comparable.
    stats->frontend_us_avg =
        g_frontend_count ? (float)g_frontend_us_total / (float)g_frontend_count : 0.0f;

    // Real-time factor: microseconds of CPU per invoke against the 30 ms of audio
    // an invoke consumes. Anything at or above 1.0 means the detector cannot keep up.
    const float per_invoke_us = stats->invoke_us_avg +
                                stats->frontend_us_avg * (float)kFeatureSlicesPerInvoke;
    stats->realtime_factor =
        per_invoke_us / (float)(kFeatureSlicesPerInvoke * kWindowStepMs * 1000);
}

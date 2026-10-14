// microWakeWord streaming detector: TFLM microfrontend features -> int8 streaming
// TFLite model -> sliding-window probability average.

#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// The frontend consumes audio in fixed 10 ms chunks.
#define WAKE_WORD_SAMPLE_RATE     16000
#define WAKE_WORD_STEP_MS         10
#define WAKE_WORD_SAMPLES_PER_STEP (WAKE_WORD_SAMPLE_RATE / 1000 * WAKE_WORD_STEP_MS)  // 160

/* 返回 0 成功,-1 失败(原 esp_err_t, 替换 ESP_OK/ESP_FAIL) */
int wake_word_init(void);

// Feeds exactly WAKE_WORD_SAMPLES_PER_STEP samples. Returns true on the step where
// the wake word fires; the detector then stays silent for the refractory period.
bool wake_word_process(const int16_t *samples);

// Most recent averaged probability, for logging.
float wake_word_last_probability(void);

// Drops all frontend and model state (used after a detection).
void wake_word_reset(void);

// Performance statistics: timing and memory.
typedef struct {
    uint32_t arena_used_bytes;
    uint32_t arena_total_bytes;
    uint32_t invoke_count;
    float invoke_us_avg;
    int64_t invoke_us_max;
    float frontend_us_avg;
    float realtime_factor;
} wake_word_stats_t;

void wake_word_get_stats(wake_word_stats_t *stats);

#ifdef __cplusplus
}
#endif

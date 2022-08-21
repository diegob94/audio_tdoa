#ifndef AUDIO_HEADER_GUARD
#define AUDIO_HEADER_GUARD

#include "esp_err.h"

#define I2S_NUM     0
#define I2S_BCK_IO  0
#define I2S_WS_IO   2
#define I2S_DI_IO   4
#define SAMPLE_RATE 48000

#define BUFFER_SAMPLE_SIZE 256

esp_err_t audio_init(void);
esp_err_t audio_read(int32_t *, size_t, size_t *);

#endif // AUDIO_HEADER_GUARD

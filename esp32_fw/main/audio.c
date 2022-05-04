#include "audio.h"
#include "driver/i2s.h"
#include "esp_err.h"
#include "hal/i2s_types.h"
#include "esp_log.h"
#include "soc/i2s_reg.h"

static const char *TAG = "audio";

esp_err_t audio_read(int32_t *dest, size_t size, size_t *bytes_read){
    esp_err_t r = i2s_read(I2S_NUM, dest, size*2, bytes_read, portMAX_DELAY);
    ESP_LOGI(TAG,"audio_read %u bytes", *bytes_read);
    return r;
}

esp_err_t audio_init(void){

    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_RX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .dma_buf_count = 8, // dma queue length, buffer count, number of buffer
        .dma_buf_len = 64, // number of frames, length of each buffer
        .use_apll = false,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1                                //Interrupt level 1
    };

    i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_PIN_NO_CHANGE,
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_PIN_NO_CHANGE,
        .data_in_num = I2S_DI_IO
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);   //install and start i2s driver
    i2s_set_pin(I2S_NUM, &pin_config);
    REG_SET_BIT(I2S_TIMING_REG(I2S_NUM),BIT(9)); // Delay DATA_IN by one clock cycle, needed for SPH0645LM4H

    return ESP_OK;
}


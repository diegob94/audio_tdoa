#include "audio.h"
#include "driver/i2s.h"
#include "esp_err.h"

esp_err_t audio_read(void *dest, size_t size, size_t *bytes_read){
    return i2s_read(I2S_NUM, dest, size, bytes_read, portMAX_DELAY);
}

esp_err_t audio_init(void){

    i2s_config_t i2s_config = {
        .mode = I2S_MODE_MASTER | I2S_MODE_TX,
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .dma_buf_count = 6,
        .dma_buf_len = 60,
        .use_apll = false,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1                                //Interrupt level 1
    };

    i2s_pin_config_t pin_config = {
        .mck_io_num = I2S_PIN_NO_CHANGE,
        .bck_io_num = I2S_BCK_IO,
        .ws_io_num = I2S_WS_IO,
        .data_out_num = I2S_DO_IO,
        .data_in_num = I2S_DI_IO                                               //Not used
    };

    i2s_driver_install(I2S_NUM, &i2s_config, 0, NULL);   //install and start i2s driver
    i2s_set_pin(I2S_NUM, &pin_config);
    uint32_t bits_cfg = (I2S_BITS_PER_CHAN_32BIT << 16) | I2S_BITS_PER_SAMPLE_16BIT;
    i2s_set_clk(I2S_NUM, 22050, bits_cfg, I2S_CHANNEL_STEREO);

    return ESP_OK;
}

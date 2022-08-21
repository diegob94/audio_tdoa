#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include <string.h>

#include "wifi.h"
#include "gpio.h"
#include "client.h"
#include "audio.h"

static const char *TAG = "main";

void app_main(void) {
//    esp_log_level_set("*", ESP_LOG_INFO);
//    esp_log_level_set("WEBSOCKET_CLIENT", ESP_LOG_DEBUG);
//    esp_log_level_set("TRANSPORT_WS", ESP_LOG_DEBUG);
//    esp_log_level_set("TRANS_TCP", ESP_LOG_DEBUG);

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    my_gpio_init();

    gpio_set_blink_period(500);
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    esp_err_t wifi_status = wifi_init_sta();
    if(wifi_status == ESP_OK){
        gpio_set_blink_period(-1);
    } else {
        gpio_set_blink_period(100);
        ESP_LOGI(TAG,"wifi error deleting app_main task");
        vTaskDelete(NULL);
    }
    ESP_LOGI(TAG,"wifi success");

    client_init();
    //client_test();
    audio_init();

    int64_t sync_time;
    int32_t samples[BUFFER_SAMPLE_SIZE];
    int16_t buffer[(sizeof(sync_time)/2) + BUFFER_SAMPLE_SIZE];
    size_t bytes_read = 0;
    while(true){
        audio_read(samples, BUFFER_SAMPLE_SIZE, &bytes_read);
        sync_time = gpio_get_sync_time();
        ESP_LOGI(TAG,"timestamp = %lld",sync_time);
        buffer[0] = sync_time & 0xFFFF;
        buffer[1] = (sync_time >> 16) & 0xFFFF;
        buffer[2] = (sync_time >> (2*16)) & 0xFFFF;
        buffer[3] = (sync_time >> (3*16)) & 0xFFFF;
        for(int i = 0; i < bytes_read/2; i++){
            buffer[i + (sizeof(sync_time)/2)] = samples[i]>>16;
        }
        client_send(buffer,sizeof(buffer));
        vTaskDelay(10);
//        vTaskSuspend(NULL);
    }
}


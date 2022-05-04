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
    gpio_init_led();
    //gpio_init_sync_test_input();
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

    int32_t samples[256];
    int16_t buffer[256];
    size_t bytes_read = 0;
    while(true){
        audio_read(samples, 256, &bytes_read);
        for(int i = 0; i < bytes_read/2; i++){
            buffer[i] = samples[i]>>16;
        }
        client_send(buffer,bytes_read/2);
        vTaskDelay(10);
    }
}


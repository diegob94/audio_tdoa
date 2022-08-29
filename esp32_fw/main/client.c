#include "client.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/message_buffer.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"

#include "sdkconfig.h"

static const char *TAG = "client";

MessageBufferHandle_t send_xMessageBuffer = NULL;
static TaskHandle_t clientHandle = NULL;

static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    //esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    switch (event_id) {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_CONNECTED");
        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        break;
    case WEBSOCKET_EVENT_DATA:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_DATA");
        //ESP_LOGI(TAG, "Received opcode=%d", data->op_code);
        //if (data->op_code == 0x08 && data->data_len == 2) {
        //    ESP_LOGW(TAG, "Received closed message with code=%d", 256*data->data_ptr[0] + data->data_ptr[1]);
        //} else {
        //    ESP_LOGW(TAG, "Received=%.*s", data->data_len, (char *)data->data_ptr);
        //}
        //ESP_LOGW(TAG, "Total payload length=%d, data_len=%d, current payload offset=%d\r\n", data->payload_len, data->data_len, data->payload_offset);

        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGI(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}
#define _GET_WS_URI(HOST,PORT,ID) "ws://"HOST":"#PORT"/tx"#ID
#define GET_WS_URI(HOST,PORT,ID) _GET_WS_URI(HOST, PORT, ID)

static void ws_client_task(void *pvParameters){
    esp_websocket_client_config_t websocket_cfg = {};

    websocket_cfg.uri = GET_WS_URI(CONFIG_TDOA_SERVER_IPV4_ADDR,CONFIG_TDOA_SERVER_PORT,CONFIG_TDOA_SIGNAL_ID);

    while(true){
        ESP_LOGI(TAG, "Main loop: Connecting to URL: %s", websocket_cfg.uri);

        esp_websocket_client_handle_t client = esp_websocket_client_init(&websocket_cfg);
        esp_websocket_register_events(client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)client);

        esp_websocket_client_start(client);
        char buf[1024];
        while(true){
            ESP_LOGI(TAG,"send loop: start");
            size_t len = xMessageBufferReceive(send_xMessageBuffer,buf,sizeof(buf),portMAX_DELAY);
            buf[len] = 0;
            ESP_LOGI(TAG,"send loop: dequed message (%u bytes)",len);
            while(true){
                if(esp_websocket_client_is_connected(client)){
                    esp_websocket_client_send_bin(client, buf, len, portMAX_DELAY);
                    break;
                }
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
        }
        esp_websocket_client_close(client, portMAX_DELAY);
        ESP_LOGI(TAG, "Main loop: Websocket Stopped, re-opening...");
        esp_websocket_client_destroy(client);
    }
    vTaskDelete(NULL);
}

esp_err_t client_send(void* buf, size_t buf_size){
    ESP_LOGI(TAG,"client_send %u bytes", buf_size);
    size_t len = xMessageBufferSend(send_xMessageBuffer,buf,buf_size,portMAX_DELAY);
    if(len == 0) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

esp_err_t client_init(void) {
    send_xMessageBuffer = xMessageBufferCreate(4096);
    xTaskCreate(ws_client_task, "ws_client", 4096, NULL, 5, &clientHandle);
    return ESP_OK;
}

static void ws_client_test(void *pvParameters){
    char buf[100];
    for(int i = 0; i<10000; i++){
        sprintf(buf,"i = %d\n",i);
        ESP_LOGI(TAG,"ws_client_test: Send: %s",buf);
        client_send(buf,strlen(buf));
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);
}

void client_test(void) {
    xTaskCreate(ws_client_test, "ws_client_test", 4096, NULL, 5, NULL);
}


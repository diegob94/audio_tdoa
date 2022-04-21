#include "client.h"

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/message_buffer.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"

static const char *TAG = "client";
static int sock;
static TaskHandle_t clientHandle = NULL;

static void tcp_client_task(void *pvParameters){
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    while (1) {
        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(host_ip);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            break;
        }
        ESP_LOGI(TAG, "Successfully connected");

        xTaskNotifyWait(0,0,NULL,portMAX_DELAY); 

        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }
    }
    vTaskDelete(NULL);
}

esp_err_t client_send(void* buf, size_t buf_size){
    if(sock == -1) {
        return ESP_FAIL;
    }
    int err = send(sock, buf, buf_size, 0);
    if (err < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
        xTaskNotify(clientHandle, 0, eNoAction);
        return ESP_FAIL;
    }
    return ESP_OK;
}

ssize_t client_recv(char* buf, size_t buf_size){
    if(sock == -1) {
        return ESP_FAIL;
    }
    ssize_t len = recv(sock, buf, buf_size, 0);
    if (len < 0) {
        ESP_LOGE(TAG, "Error ocurred during receive: errno %d", errno);
        xTaskNotify(clientHandle, 0, eNoAction);
    }
    return len;
}

esp_err_t client_init(void) {
    sock = -1;
    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, &clientHandle);
    return ESP_OK;
}

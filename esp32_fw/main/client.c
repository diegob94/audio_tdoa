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
static int sock = -1;
static TaskHandle_t clientHandle = NULL;
static TaskHandle_t send_taskHandle = NULL;
static TaskHandle_t recv_taskHandle = NULL;
static void send_task(void *pvParameters);
static void recv_task(void *pvParameters);
MessageBufferHandle_t recv_xMessageBuffer = NULL;
MessageBufferHandle_t send_xMessageBuffer = NULL;

static void tcp_client_task(void *pvParameters){
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    while (1) {
        if (sock != -1) {
            ESP_LOGE(TAG, "Shutting down socket and restarting...");
            shutdown(sock, 0);
            close(sock);
        }

        struct sockaddr_in dest_addr;
        dest_addr.sin_addr.s_addr = inet_addr(host_ip);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
        sock = socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
            continue;
        }
        ESP_LOGI(TAG, "Socket created, connecting to %s:%d", host_ip, PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_in6));
        if (err != 0) {
            ESP_LOGE(TAG, "Socket unable to connect: errno %d", errno);
            continue;
        }
        ESP_LOGI(TAG, "Successfully connected (%d)",err);

        vTaskResume(send_taskHandle);
        vTaskResume(recv_taskHandle);

        vTaskSuspend(NULL);
    }
    vTaskDelete(NULL);
}

static void send_task(void *pvParameters){
    vTaskSuspend(NULL);
    char buf[1024];
    while(1){
        size_t len = xMessageBufferReceive(send_xMessageBuffer,buf,sizeof(buf),portMAX_DELAY);
        buf[len] = 0;
        ESP_LOGI(TAG,"send_task (%d): %s",len,buf);
        int err = send(sock, buf, len, 0);
        if (err < 0) {
            ESP_LOGE(TAG, "send_task: Socket error: errno %d", errno);
            vTaskResume(clientHandle);
            vTaskSuspend(NULL);
        }
    }
    vTaskDelete(NULL);
}

static void recv_task(void *pvParameters){
    vTaskSuspend(NULL);
    char buf[1024];
    while(1){
        ssize_t len = recv(sock, buf, sizeof(buf), 0);
        buf[len] = 0;
        ESP_LOGI(TAG,"recv_task (%d): %s",len,buf);
        if (len < 0) {
            ESP_LOGE(TAG, "recv_task: Socket error: errno %d", errno);
            vTaskResume(clientHandle);
            vTaskSuspend(NULL);
            continue;
        }
        xMessageBufferSend(recv_xMessageBuffer,buf,len,portMAX_DELAY);
    }
    vTaskDelete(NULL);
}

esp_err_t client_send(void* buf, size_t buf_size){
    size_t len = xMessageBufferSend(send_xMessageBuffer,buf,buf_size,portMAX_DELAY);
    if(len == 0) {
        return ESP_FAIL;
    }
    return ESP_OK;
}

size_t client_recv(char* buf, size_t buf_size){
    size_t len = xMessageBufferReceive(recv_xMessageBuffer,buf,buf_size,portMAX_DELAY);
    return len;
}

esp_err_t client_init(void) {
    recv_xMessageBuffer = xMessageBufferCreate(4096);
    send_xMessageBuffer = xMessageBufferCreate(4096);
    xTaskCreate(send_task, "tcp_client_send", 2*4096, NULL, 5, &send_taskHandle);
    xTaskCreate(recv_task, "tcp_client_recv", 2*4096, NULL, 5, &recv_taskHandle);
    xTaskCreate(tcp_client_task, "tcp_client", 4096, NULL, 5, &clientHandle);
    return ESP_OK;
}

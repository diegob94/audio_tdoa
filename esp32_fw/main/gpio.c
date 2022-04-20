#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "gpio.h"

static const char *TAG = "gpio";
static TaskHandle_t ledHandle = NULL;
static void gpio_led_blink_task(void *);
static TaskHandle_t ISRHandle = NULL;
static void button_isr_handler(void*);
static void gpio_sync_test_task(void *);
static long long int tsf_time = 0;

esp_err_t gpio_init_led(void) {
    gpio_config_t io_conf = {
        //disable interrupt
        .intr_type = GPIO_INTR_DISABLE,
        //set as output mode
        .mode = GPIO_MODE_OUTPUT,
        //bit mask of the pins that you want to set,e.g.GPIO18/19
        .pin_bit_mask = GPIO_LED_PIN_SEL,
        //disable pull-down mode
        .pull_down_en = 0,
        //disable pull-up mode
        .pull_up_en = 0,
    };
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    xTaskCreate(gpio_led_blink_task, "gpio_led_blink_task", 4096, NULL, 1, &ledHandle);
    return ESP_OK;
}

esp_err_t gpio_init_sync_test_input(void) {
    gpio_config_t io_conf = {
        //disable interrupt
        .intr_type = GPIO_INTR_ANYEDGE,
        //set as output mode
        .mode = GPIO_MODE_INPUT,
        //bit mask of the pins that you want to set,e.g.GPIO18/19
        .pin_bit_mask = GPIO_SYNC_TEST_PIN_SEL,
        //disable pull-down mode
        .pull_down_en = 0,
        //disable pull-up mode
        .pull_up_en = 0,
    };
    //configure GPIO with the given settings
    gpio_config(&io_conf);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_SYNC_TEST_PIN_NUM, button_isr_handler, NULL);
    xTaskCreate(gpio_sync_test_task, "gpio_sync_test_task", 4096, NULL , 10, &ISRHandle);
    return ESP_OK;
}

void gpio_set_blink_period(int32_t period) {
    ESP_LOGD(TAG,"gpio_set_blink_period: %d",period);
    xTaskNotify(ledHandle, period, eSetValueWithOverwrite);
}

esp_err_t gpio_set_led(bool state) {
    return gpio_set_level(GPIO_LED_PIN_NUM,!state);
}

static void gpio_led_blink_task(void *pvParameters) {
    ESP_LOGD(TAG, "gpio_led_blink_task start");
    int32_t period = -1;
    uint32_t notify_value = 0;
    bool state = 0;
    while(true) {
        if(notify_value != 0) {
            period = notify_value;
        }
        if(period == -1) {
            state = 1;
        } else {
            state = !state;
        }
        if(period == -1) {
            period = pdMS_TO_TICKS(1000*60*60);
        }
        gpio_set_led(state);
        ESP_LOGD(TAG, "gpio_led_blink_task wait start. period: %d",period);
        xTaskNotifyWait(ULONG_MAX,
                        ULONG_MAX,
                        &notify_value,
                        pdMS_TO_TICKS(period)); 
        ESP_LOGD(TAG, "gpio_led_blink_task wait end. notify_value: %d",notify_value);
    }
    vTaskDelete(NULL);
}

static void button_isr_handler(void* arg) {
    tsf_time = esp_wifi_get_tsf_time(WIFI_IF_STA);
    xTaskResumeFromISR(ISRHandle);
}

static void gpio_sync_test_task(void *arg){
    while(true) {
        vTaskSuspend(NULL);
        ESP_LOGI(TAG,"TSF time = %lld",tsf_time);
    }
}

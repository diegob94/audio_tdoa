#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_mesh.h"

#include "gpio.h"
#include "hal/gpio_types.h"

#include "audio.h"

static const char *TAG = "gpio";
static TaskHandle_t ledHandle = NULL;
static void gpio_led_blink_task(void *);
static void sync_input_isr_handler(void*);
static int64_t tsf_time = 0;
static int sample_count = 0;

esp_err_t my_gpio_init(void) {
    gpio_config_t led_io_conf = {
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
    gpio_config_t sync_io_conf = {
        //disable interrupt
        .intr_type = GPIO_INTR_NEGEDGE,
        //set as output mode
        .mode = GPIO_MODE_INPUT,
        //bit mask of the pins that you want to set,e.g.GPIO18/19
        .pin_bit_mask = GPIO_SYNC_PIN_SEL,
        //disable pull-down mode
        .pull_down_en = 0,
        //disable pull-up mode
        .pull_up_en = 0,
    };
    //configure GPIO with the given settings
    gpio_config(&led_io_conf);
    gpio_config(&sync_io_conf);
    xTaskCreate(gpio_led_blink_task, "gpio_led_blink_task", 4096, NULL, 1, &ledHandle);
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(GPIO_SYNC_PIN_NUM, sync_input_isr_handler, NULL);
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

static void sync_input_isr_handler(void* arg) {
    if(sample_count==0){
        tsf_time = esp_mesh_get_tsf_time();
    }
    if(sample_count++ == BUFFER_SAMPLE_SIZE){
        sample_count = 0;
    }
}

int64_t gpio_get_sync_time(){
    return tsf_time;
}


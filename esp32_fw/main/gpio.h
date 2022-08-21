#ifndef GPIO_HEADER_GUARD
#define GPIO_HEADER_GUARD

#include "esp_err.h"

#define GPIO_LED_PIN_SEL GPIO_SEL_22
#define GPIO_LED_PIN_NUM GPIO_NUM_22
#define GPIO_SYNC_PIN_SEL GPIO_SEL_16
#define GPIO_SYNC_PIN_NUM GPIO_NUM_16
#define ESP_INTR_FLAG_DEFAULT 0
#define TIMESTAMP_BYTE_SIZE sizeof(int64_t)

esp_err_t my_gpio_init(void);
esp_err_t gpio_test_wifi_sync(void);
esp_err_t gpio_set_led(bool);
void gpio_set_blink_period(int32_t);
int64_t gpio_get_sync_time(void);

#endif // GPIO_HEADER_GUARD

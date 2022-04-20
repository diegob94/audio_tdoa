#ifndef GPIO_HEADER_GUARD
#define GPIO_HEADER_GUARD

#include "esp_err.h"

#define GPIO_LED_PIN_SEL GPIO_SEL_22
#define GPIO_LED_PIN_NUM GPIO_NUM_22
#define GPIO_SYNC_TEST_PIN_SEL GPIO_SEL_13
#define GPIO_SYNC_TEST_PIN_NUM GPIO_NUM_13
#define ESP_INTR_FLAG_DEFAULT 0

esp_err_t gpio_init_led(void);
esp_err_t gpio_init_sync_test_input(void);
esp_err_t gpio_set_led(bool);
void gpio_set_blink_period(int32_t);

#endif // GPIO_HEADER_GUARD

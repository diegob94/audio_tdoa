#ifndef WIFI_HEADER_GUARD
#define WIFI_HEADER_GUARD

#define ESP_WIFI_SSID      "myssid"
#define ESP_WIFI_PASSWORD  "mypassword"
#define ESP_MAXIMUM_RETRY  10

esp_err_t wifi_init_sta(void);

#endif // WIFI_HEADER_GUARD

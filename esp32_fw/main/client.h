#ifndef CLIENT_HEADER_GUARD
#define CLIENT_HEADER_GUARD

#include "esp_err.h"

esp_err_t client_init(void);
esp_err_t client_send(void*, size_t);
void client_test(void);

#endif // CLIENT_HEADER_GUARD

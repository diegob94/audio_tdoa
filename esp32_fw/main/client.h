#ifndef CLIENT_HEADER_GUARD
#define CLIENT_HEADER_GUARD

#include "esp_err.h"
#include "sdkconfig.h"

#define HOST_IP_ADDR CONFIG_TDOA_SERVER_IPV4_ADDR
#define PORT         CONFIG_TDOA_SERVER_PORT

esp_err_t client_init(void);
esp_err_t client_send(void*, size_t);
size_t client_recv(char*, size_t);

#endif // CLIENT_HEADER_GUARD

#pragma once

#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "common.h"

#include "esp_tls.h"
#include "esp_netif.h"
#include "esp_http_client.h"

#include "cJSON.h"

#include "device.h"

#define HTTP_CLIENT_TAG         "HTTP_CLIENT_TAG"
#define MAX_HTTP_RECV_BUFFER    4096
#define MAX_HTTP_OUTPUT_BUFFER  4096

esp_err_t   _http_event_handler(esp_http_client_event_t *evt);
device_t    register_device(void);
device_t    registerDevice_json_parse(char *device_json_str);
void        send_presence_state(char *state);

#endif
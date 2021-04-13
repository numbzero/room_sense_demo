#include "http_client.h"

esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  // Buffer to store response of http request from event handler
    static int output_len;       // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            // if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(HTTP_CLIENT_TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            // }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(HTTP_CLIENT_TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(HTTP_CLIENT_TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(HTTP_CLIENT_TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                if (output_buffer != NULL) {
                    free(output_buffer);
                    output_buffer = NULL;
                }
                ESP_LOGI(HTTP_CLIENT_TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(HTTP_CLIENT_TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}

/*
 * http://host:8088/y4se/registerDevice
 * 
 * Headers:
 *      Content-Type: application/json
 * Body:
 *      {
 *        "name": "name_of_device",
 *        "macAddress": "01:02:03:04:05:06"
 *      }
 */
device_t        register_device(void)
{
    device_t    device;
    char        local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    int         status_code;
    int         content_length;
    uint8_t     mac_addr[6]    = {0};
    char        post_data[256] = {0};

    esp_http_client_config_t    config = {
        .host = "192.168.0.4",
        .port = 8088,
        .method = HTTP_METHOD_POST,
        .buffer_size = 2048,
        .path = "/y4se/registerDevice",
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,  // Address of local buffer to get response
        .disable_auto_redirect = true,
        .max_redirection_count = 10
    };

    // Get MAC address
    ESP_ERROR_CHECK(esp_efuse_mac_get_default(mac_addr));
    sprintf(post_data, "{\"name\":\"esp32_test_device\",\"macAddress\":\"%x:%x:%x:%x:%x:%x\"}", mac_addr[0],
                                                                                                mac_addr[1],
                                                                                                mac_addr[2],
                                                                                                mac_addr[3],
                                                                                                mac_addr[4],
                                                                                                mac_addr[5]);                                                                                           
    // Start a HTTP session
    esp_http_client_handle_t    client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_err_t   err = esp_http_client_perform(client);

    if (err == ESP_OK || err == ESP_ERR_HTTP_MAX_REDIRECT)
    {
        status_code = esp_http_client_get_status_code(client);
        content_length = esp_http_client_get_content_length(client);
        printf("[*] http://%s:%d%s - %d | %d\n\n", config.host, config.port, config.path, status_code, content_length);
    }
    else
        ESP_LOGE(HTTP_CLIENT_TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    // ESP_LOG_BUFFER_HEX(HTTP_CLIENT_TAG, local_response_buffer, strlen(local_response_buffer));
    printf("length - %d\n\n", strlen(local_response_buffer));
    printf("%s\n", local_response_buffer);
    printf("\n");
    /*
     *  Parse response JSON
     */
    device = registerDevice_json_parse(local_response_buffer);
    ESP_ERROR_CHECK(esp_http_client_cleanup(client));
    return device;
}

device_t    registerDevice_json_parse(char *device_json_str)
{
    device_t    device;

    device.id = -1;
    memset(device.name, 0, 64);
    memset(device.mac_addr, 0, 32);
    memset(device.token, 0, 512);
    memset(device.otp, 0, 32);

    cJSON   *device_json = cJSON_Parse(device_json_str);
    if (device_json)
    {
        const cJSON *id;
        const cJSON *name;
        const cJSON *mac_addr;
        const cJSON *token;
        const cJSON *otp;

        id       = cJSON_GetObjectItemCaseSensitive(device_json, "id");
        name     = cJSON_GetObjectItemCaseSensitive(device_json, "name");
        mac_addr = cJSON_GetObjectItemCaseSensitive(device_json, "macAddress");
        token    = cJSON_GetObjectItemCaseSensitive(device_json, "token");
        otp      = cJSON_GetObjectItemCaseSensitive(device_json, "otp");

        if (cJSON_IsNumber(id) && (id->valueint > -1))
            device.id = id->valueint;
        if (cJSON_IsString(name) && (name->valuestring != NULL))
            memcpy(device.name, name->valuestring, strlen(name->valuestring) + 1);
        if (cJSON_IsString(mac_addr) && (mac_addr->valuestring != NULL))
            memcpy(device.mac_addr, mac_addr->valuestring, strlen(mac_addr->valuestring) + 1);
        if (cJSON_IsString(token) && (token->valuestring != NULL))
            memcpy(device.token, token->valuestring, strlen(token->valuestring) + 1);
        if (cJSON_IsString(otp) && (otp->valuestring != NULL))
            memcpy(device.otp, otp->valuestring, strlen(otp->valuestring) + 1);

        cJSON_Delete(device_json);
    }
    return device;    
}

void    send_presence_state(char *state)
{
    char        local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};
    int         status_code;
    char        post_data[256] = {0};
    char        path[128] = {0};

    sprintf(path, "/y4se/device/%d/present", esp32_device.id);
    sprintf(post_data, "isPresent=%s", state);      

    esp_http_client_config_t    config = {
        .host = "192.168.0.4",
        .port = 8088,
        .method = HTTP_METHOD_POST,
        .buffer_size = 2048,
        .path = strdup(path),
        .event_handler = _http_event_handler,
        .user_data = local_response_buffer,  // Address of local buffer to get response
        .disable_auto_redirect = false,
        .max_redirection_count = 10
    };
                                                                                          
    esp_http_client_handle_t    client = esp_http_client_init(&config);
    esp_http_client_set_header(client, "Content-Type", "application/x-www-form-urlencoded");
    esp_http_client_set_header(client, "Authorization", esp32_device.token);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_err_t                   err = esp_http_client_perform(client);

    if (err == ESP_OK || err == ESP_ERR_HTTP_MAX_REDIRECT)
    {
        status_code = esp_http_client_get_status_code(client);
        printf("[*] http://%s:%d%s - %d | %s \n", config.host, config.port, config.path, status_code, state);
    }
    else
        ESP_LOGE(HTTP_CLIENT_TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    ESP_ERROR_CHECK(esp_http_client_cleanup(client));
}
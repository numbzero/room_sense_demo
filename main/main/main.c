#include "common.h"
#include "http_client.h"
#include "wifi_manager.h"
#include "http_app.h"
#include "device.h"
#include "sensors.h"

static esp_err_t    my_get_handler(httpd_req_t *req)
{
	if (strcmp(req->uri, "/otp_mac") == 0)
    {
		ESP_LOGI("OTP_MAC", "Serving page /otp_mac");
		char response[256];
        // esp32_device = register_device();
        sprintf(response, "<html><body><h1>MAC: %s</h2><h1>OTP: %s</h2></body></html>",
                esp32_device.mac_addr, esp32_device.otp);
		httpd_resp_set_status(req, "200 OK");
		httpd_resp_set_type(req, "text/html");
		httpd_resp_send(req, response, strlen(response));
	}
	else
		/* send a 404 otherwise */
		httpd_resp_send_404(req);
	return ESP_OK;
}

void        after_got_ip(void *pvParameters)
{
    esp32_device = register_device();

    http_app_set_handler_hook(HTTP_GET, &my_get_handler);
        // Create presence detection task
    xTaskCreate(presence_detection, "presence_detection", 2048 * 10, NULL, 1, NULL);
}

void    app_main(void)
{
    printf("START!\n");
    on_board_led_setup();
    pir_sensor_setup();
    // Create blink led task just for check
    xTaskCreate(blink_led  /* Pointer to the task function */,
                "blink_led" /* Descriptive name */,
                CONFIG_ESP_MINIMAL_SHARED_STACK_SIZE /* Stack size */,
                NULL /* Parameter to pass */,
                1 /* Task priority */,
                NULL /* Passed back handle */
    );
    init_nvs();
    
    wifi_manager_start();

    wifi_manager_set_callback(WM_EVENT_STA_GOT_IP, &after_got_ip);
}

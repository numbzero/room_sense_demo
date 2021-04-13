#include "common.h"

void    on_board_led_setup(void)
{
    ESP_LOGD(COMMON_TAG, "on_board_led_setup");
    
    gpio_config_t gpio_2;

    gpio_2.pin_bit_mask = (1ULL << 2);
    gpio_2.mode         = GPIO_MODE_OUTPUT;
    gpio_2.pull_up_en   = GPIO_PULLUP_DISABLE;   // 0x0 - Disable GPIO pull-up resistor
    gpio_2.pull_down_en = GPIO_PULLDOWN_DISABLE; // 0x0 - Disable GPIO pull-down resistor
    gpio_2.intr_type    = GPIO_INTR_DISABLE;     // 0 - Disable GPIO interrupt
    gpio_config(&gpio_2);

    ESP_LOGI(COMMON_TAG, "LED (GPIO_2) is configgured");
}

void    blink_led(void *pvParameters)
{
    ESP_LOGI(COMMON_TAG, "blink_led task");
    for (;;)
    {
        gpio_set_level(LED, 1); // LED on
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        gpio_set_level(LED, 0); // LED off
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void    print_something(void *pvParameters)
{
    ESP_LOGI(COMMON_TAG, "print_something task");
    for (;;)
    {
        printf("something :P\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        printf("something :D\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void    init_nvs(void)
{
    // Initialize NVS
    ESP_LOGI(NVS_TAG, "init_nvs");
    esp_err_t   ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);    
}
#include "sensors.h"

void    pir_sensor_setup(void)
{
    ESP_LOGI(PIR_SENSOR_TAG, "pir_sensor_setup");
    gpio_config_t pir_sensor;

    pir_sensor.pin_bit_mask = (1UL << PIR_SENSOR);
    pir_sensor.mode         = GPIO_MODE_INPUT;
    pir_sensor.pull_up_en   = GPIO_PULLUP_DISABLE;
    pir_sensor.pull_down_en = GPIO_PULLDOWN_DISABLE;
    pir_sensor.intr_type    = GPIO_INTR_DISABLE;

    ESP_ERROR_CHECK(gpio_config(&pir_sensor));
}

bool    get_pir_sensor_state(void)
{
    return gpio_get_level(PIR_SENSOR);
}

void    presence_detection(void *pvParameters)
{   
    bool    prev_state   = false;
    bool    sensor_state = false;
   
    ESP_LOGI(PIR_SENSOR_TAG, "presence detection task");
    for (;;)
    {
        sensor_state = get_pir_sensor_state();
        if (sensor_state != prev_state)
        {
            prev_state = sensor_state;
            if (sensor_state)
            {
                printf("Presence detected!\n");
                send_presence_state("true");
            }
            else
            {
                printf("No presence detected\n");
                send_presence_state("false");
            }
            // send_presence_state(sensor_state);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    
    }
}
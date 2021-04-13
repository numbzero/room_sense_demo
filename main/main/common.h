#pragma once
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "esp_log.h"
#include "esp_err.h"
#include "esp_system.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "nvs_flash.h"

#define COMMON_TAG  "COMMON_TAG"
#define NVS_TAG     "NVS_TAG"
#define LED         2

void    on_board_led_setup(void);
void    blink_led(void *pvParameters);
void    print_something(void *pvParameters);
void    init_nvs(void);

#endif
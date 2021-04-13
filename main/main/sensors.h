#pragma once
#ifndef SENSOR_H
#define SENSOR_H

#include "common.h"
#include "http_client.h"
#include "driver/gpio.h"

#define PIR_SENSOR_TAG  "PIR_SENSOR_TAG"
#define PIR_SENSOR      GPIO_NUM_17

void    pir_sensor_setup(void);
bool    get_pir_sensor_state(void);
void    presence_detection(void *pvParameters);

#endif
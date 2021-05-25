#pragma once
#include "driver/ledc.h"
#include "driver/gpio.h"

//ultrasonic sensor config
const gpio_num_t TRIG = GPIO_NUM_33;
const gpio_num_t ECHO = GPIO_NUM_14;
const gpio_num_t secondEcho = GPIO_NUM_12;
const ledc_channel_t SENSOR_PWM = LEDC_CHANNEL_0;

//leds config
const uint maxDistance = 200; // 200cm
const uint ledCount = 30;
const uint inactiveLedCount = 10; 
const gpio_num_t ledPin = GPIO_NUM_27;
const uint8_t dModeBrig = 7; // 7/255
const uint8_t actBrig = 127; // 127/255
const ledc_channel_t ledPwmChannel = LEDC_CHANNEL_1;
const uint activeTimeout = 4000; // 4s
const uint activeEpsilon = 25; // 25cm


const gpio_num_t simpleLed = GPIO_NUM_25;

//buzzer config
const gpio_num_t buzzerPin = GPIO_NUM_32;
volatile int buzzerDelTime = -1;
const uint continuousSignalThreshold = 80;  // 80%


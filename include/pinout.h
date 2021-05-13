#ifndef PINOUT_HH
#define PINOUT_HH

#include <Arduino.h>

//ultrasonic sensor config
const gpio_num_t TRIG = GPIO_NUM_33;
const gpio_num_t ECHO = GPIO_NUM_14;
const gpio_num_t secondEcho = GPIO_NUM_12;

//leds config
const uint maxDistance = 200;
const uint ledCount = 30;
const gpio_num_t ledPin = GPIO_NUM_27;
const int darkModeBrightness = 7;
const int activeBrightness = 127;

const gpio_num_t simpleLed = GPIO_NUM_25;

//buzzer config
const gpio_num_t buzzerPin = GPIO_NUM_32;
int buzzerDelTime = -1;

#endif
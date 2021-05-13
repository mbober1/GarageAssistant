#include "ultrasonic.h"

extern QueueHandle_t distanceQueue;
extern QueueHandle_t secondQueue;
unsigned long sensor1Time;
unsigned long sensor2Time;

#define ECHO GPIO_NUM_14
#define secondEcho GPIO_NUM_12

uint8_t errorDistaneCounter = 0; // Liczenie błędów - jeśli jest ponad 30 pod rząd oznacza to brak obiektu w zakresie.
uint8_t secondErrorDistaneCounter = 0;

/************************************************************************************/

static void IRAM_ATTR triggerInterrupt(void* arg)
{

    if(gpio_get_level(ECHO)) {

        uint distance = (esp_timer_get_time() - sensor1Time)/58;

        if(distance < 200) {

            errorDistaneCounter = 0;
            xQueueSendToBack(distanceQueue, &distance, 0);
        }
        else {

            errorDistaneCounter++;
            if(errorDistaneCounter > 30) {
                distance = 200;
                xQueueSendToBack(distanceQueue, &distance, 0);
            }
        }

    }
    else sensor1Time = esp_timer_get_time();
}

/************************************************************************************/

static void IRAM_ATTR secondInterrupt(void* arg)
{

    if(gpio_get_level(secondEcho)) {

        uint distance = (esp_timer_get_time() - sensor2Time)/58;

        if(distance < 200) {

            secondErrorDistaneCounter = 0;
            xQueueSendToBack(secondQueue, &distance, 0);
        }
        else {

            secondErrorDistaneCounter++;
            if(secondErrorDistaneCounter > 30) {
                distance = 200;
                xQueueSendToBack(secondQueue, &distance, 0);
            }
        }

    }
    else sensor2Time = esp_timer_get_time();
}

/************************************************************************************/

Ultrasonic::Ultrasonic(gpio_num_t triggerPin, gpio_num_t echoPin, ledc_channel_t pwmChannel) : triggerPin(triggerPin), echoPin(echoPin)
{
    // Echo:
    const int echo_num = (int)echoPin;
    int err = 0;

    err += gpio_set_direction(this->triggerPin, GPIO_MODE_OUTPUT);
    err += gpio_set_level(this->triggerPin, 0);

    err += gpio_install_isr_service(0);
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pin_bit_mask = (1ULL<<echo_num);
    err += gpio_config(&io_conf);

    err += gpio_isr_handler_add(this->echoPin, triggerInterrupt, (void*) &echo_num);

    // Trigger:
    ledc_timer.duty_resolution = LEDC_TIMER_15_BIT;
    ledc_timer.freq_hz = 5;
    ledc_timer.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_timer.timer_num = LEDC_TIMER_0;
    err += ledc_timer_config(&ledc_timer);

    ledc_channel.channel = pwmChannel;
    ledc_channel.duty = 2;
    ledc_channel.gpio_num = this->triggerPin;
    ledc_channel.speed_mode = LEDC_HIGH_SPEED_MODE;
    ledc_channel.hpoint     = 0;
    ledc_channel.timer_sel  = LEDC_TIMER_0;
    
    err += ledc_channel_config(&ledc_channel);

    if(err) ESP_LOGE("Ultrasonic", "sensor failed");
    else ESP_LOGE("Ultrasonic", "sensor initialized");
}

/************************************************************************************/

Ultrasonic::Ultrasonic(gpio_num_t echoPin) : echoPin(echoPin)
{
    // Echo:
    const int echo_num = (int)echoPin;
    int err = 0;

    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    io_conf.pin_bit_mask = (1ULL<<echo_num);
    err += gpio_config(&io_conf);

    err += gpio_isr_handler_add(this->echoPin, secondInterrupt, (void*) &echo_num);

    if(err) ESP_LOGE("Second Ultrasonic", "sensor failed");
    else ESP_LOGE("Second Ultrasonic", "sensor initialized");
}

/************************************************************************************/

Ultrasonic::~Ultrasonic() {}
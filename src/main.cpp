#define INCLUDE_vTaskSuspend 1

#include <Arduino.h>
#include <SmartLeds.h>
#include <ultrasonic.hpp>
#include "secrets.hpp"
#include <wifi.hh>
#include <mqtt.hh>

myWifi wifi(ssid, password);
myMQTT mqtt(project_name, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_dir);

//ultrasonic sensor config
const gpio_num_t TRIG = GPIO_NUM_4;
const gpio_num_t ECHO = GPIO_NUM_2;
const ledc_channel_t SENSOR_PWM = LEDC_CHANNEL_1;

//leds config
const uint maxDistance = 20;
const uint ledCount = 8;
const gpio_num_t ledPin = GPIO_NUM_12;
const ledc_channel_t ledPwmChannel = LEDC_CHANNEL_0;

QueueHandle_t distanceQueue;

Entity autoStatus(EntityType::binarySensor, "Auto", &mqtt);
Entity distance(EntityType::sensor, "Distance", &mqtt);


int stan = 0;
int val = 0;


static void ledTask(void*) {
  SmartLed leds(LED_WS2812B, ledCount, ledPin, ledPwmChannel);
  uint distance;

  while(1) {
    if(xQueueReceive(distanceQueue, &distance, portMAX_DELAY)) {
      uint percentage = (distance*100)/maxDistance;
      uint activeLeds = (percentage * ledCount)/100;
      printf("Distance: %d -> %d%% = %d/%d leds\n", distance, percentage, activeLeds, ledCount);

      for(uint i = 0; i < ledCount; ++i) {
        leds[i] = Rgb{(activeLeds<=i)?(uint8_t)0:(uint8_t)255,0,0};
      }
      leds.show();
    }
  }
  vTaskDelete(NULL);
}



void setup() {
  Serial.begin(115200);
  Serial.printf("Version: %d\n", LAST_BUILD_TIME);
  wifi.connect();
  mqtt.connect(wifi.client);

  autoStatus.configure();
  distance.configure();

  distanceQueue = xQueueCreate(5, sizeof(uint));
  // xTaskCreate(ledTask, "Ledy_Task", 4096, nullptr, 5, NULL); 
  // Ultrasonic sensor(TRIG, ECHO, SENSOR_PWM);
}




void loop() {
  mqtt.loop();
  autoStatus.update(stan ? "ON" : "OFF");
  stan = !stan;
  printf("MQTT auto status %d\n", stan);

  distance.update(val++);

  delay(1000);
}

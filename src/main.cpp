#define INCLUDE_vTaskSuspend 1

#include "secrets.hpp"
#include "pinout.hpp"

#include <SmartLeds.h>
#include <ultrasonic.hpp>

#include <wifi.hpp>
#include <mqtt.hpp>


myWifi wifi(ssid, password);
myMQTT mqtt(project_name, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_dir);

QueueHandle_t distanceQueue;
QueueHandle_t secondQueue;

Entity statusEntity(EntityType::binarySensor, "status", &mqtt);
Entity distanceEntity(EntityType::sensor, "distance", &mqtt);
Entity secondDistEntity(EntityType::sensor, "distance2", &mqtt);

bool stan = false;
uint distance;
uint secondDistance;

/************************************************************************************/

static void ledTask(void*) {
  SmartLed leds(LED_WS2812B, ledCount, ledPin, ledPwmChannel);
  uint lastDistance = 0;
  unsigned long noDiffTimer = millis();

  /************************************************/

  while(1) {
    
    xQueueReceive(secondQueue, &secondDistance, portMAX_DELAY);

    if(xQueueReceive(distanceQueue, &distance, portMAX_DELAY)) {

      if(abs(lastDistance - distance) > 25) {
        
        noDiffTimer = millis();
        lastDistance = distance;
      }

      uint percentage = (distance*100)/maxDistance;
      if(percentage > 100) percentage = 100;

      uint activeLeds = (percentage * ledCount)/100;
      if(activeLeds < 10) activeLeds = 10;

      // Mała wstawka dla buzzera:
      if(percentage < 10) buzzerDelTime = 10;
      else if(percentage > 80) buzzerDelTime = -1;
      else buzzerDelTime = map(percentage, 10, 80, 50, 750);
      // Koniec wstawki

    /************************************************/

      if(millis() - noDiffTimer > 4000) {

        buzzerDelTime = -1;
        
        if(!stan && percentage < 30) {
          
          statusEntity.update("ON");
          printf("MQTT auto status ON\n");
          stan = true;
        }
        if(stan && percentage > 90) {

          statusEntity.update("OFF");
          printf("MQTT auto status OFF\n");
          stan = false;
        }

        uint8_t k = darkModeBrightness*percentage/100;

        for(uint8_t i = 0; i < ledCount; ++i) {
          leds[i] = Rgb{(activeLeds<=i)?(uint8_t)0:(uint8_t)(darkModeBrightness-k),(activeLeds<=i)?(uint8_t)0:(uint8_t)k,0};
        }

      }
      else {

        uint8_t k = activeBrightness*percentage/100;

        for(uint8_t i = 0; i < ledCount; ++i) {
          leds[i] = Rgb{(activeLeds<=i)?(uint8_t)0:(uint8_t)(activeBrightness-k),(activeLeds<=i)?(uint8_t)0:(uint8_t)k,0};
        }

      }
      
      leds.show();
    }

    else delay(1);

  }
}

/************************************************************************************/

static void buzzerTask(void*) {
  
  pinMode(buzzerPin, OUTPUT);

  while(1) {
    
    int timeCopy = buzzerDelTime;

    if(timeCopy > 0) {
      
      //digitalWrite(buzzerPin, 1); Zakomentowane żeby nie nap...
      delay(100);
      digitalWrite(buzzerPin, 0);
      delay(timeCopy);
    }
    else delay(1);

  }
}

/************************************************************************************/

static void mqttTask(void*) {

  pinMode(simpleLed, OUTPUT);
  digitalWrite(simpleLed, 1);

  wifi.connect();
  mqtt.connect(wifi.client);
  
  statusEntity.configure();
  distanceEntity.configure();
  secondDistEntity.configure();

  digitalWrite(simpleLed, 0);

  while(1) {

    mqtt.loop();
    delay(300);
    distanceEntity.update(distance);
    secondDistEntity.update(secondDistance);
    Serial.printf("Main sensor: %d\n", distance);
    Serial.printf("Second sensor: %d\n", secondDistance);
  }
}

/************************************************************************************/

void setup() {

  Serial.begin(115200);
  Serial.printf("Version: %d\n", LAST_BUILD_TIME);
  
  distanceQueue = xQueueCreate(5, sizeof(uint));
  secondQueue = xQueueCreate(5, sizeof(uint));

  xTaskCreate(ledTask, "Ledy_Task", 4096, nullptr, 1, NULL);
  xTaskCreate(buzzerTask, "Buzzer_Task", 1024, nullptr, 6, NULL);
  xTaskCreate(mqttTask, "Mqtt_Task", 4096, nullptr, 6, NULL);

  Ultrasonic sensor(TRIG, ECHO, SENSOR_PWM);
  Ultrasonic secondSensor(secondEcho);
}

void loop() {}
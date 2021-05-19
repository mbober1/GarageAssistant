#define INCLUDE_vTaskSuspend 1

#include "secrets.hpp"
#include "config.hpp"

#include <SmartLeds.h>
#include <ultrasonic.hpp>

#include <wifi.hpp>
#include <mqtt.hpp>


myWifi wifi(ssid, password);
myMQTT mqtt(project_name, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_dir);

QueueHandle_t distanceQueue;
QueueHandle_t secondQueue;

Entity statusEntity(EntityType::binarySensor, "status", &mqtt);
Entity orientationEntity(EntityType::binarySensor, "apud", &mqtt);
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

  while(1) {
    xQueueReceive(secondQueue, &secondDistance, portMAX_DELAY);
    xQueueReceive(distanceQueue, &distance, portMAX_DELAY);

    if(abs(lastDistance - distance) > activeEpsilon) {
      noDiffTimer = millis();
      lastDistance = distance;
    }

    uint percentage = (distance*100)/maxDistance;
    if(percentage > 100) percentage = 100;

    uint activeLeds = (percentage * ledCount)/100;
    if(activeLeds < inactiveLedCount) activeLeds = inactiveLedCount;

    distanceEntity.update(distance);
    secondDistEntity.update(secondDistance);
    Serial.printf("Sensor1: %3d, Sensor2 %3d\n", distance, secondDistance);

    // Mała wstawka dla buzzera:
    if(percentage < 10) buzzerDelTime = 10;
    else if(percentage > continuousSignalThreshold) buzzerDelTime = -1;
    else buzzerDelTime = map(percentage, 10, 80, 50, 750);
    // Koniec wstawki


    if(millis() - noDiffTimer > activeTimeout) {

      buzzerDelTime = -1;
      
      if(!stan && percentage < 40) {
        statusEntity.update("ON");
        printf("MQTT auto status ON\n");
        stan = true;

        if(secondDistance > distance + 50) orientationEntity.update("ON");
        else orientationEntity.update("OFF");
      }

      if(stan && percentage > 80) {
        orientationEntity.update("OFF");
        statusEntity.update("OFF");
        printf("MQTT auto status OFF\n");
        stan = false;
      }

      uint8_t k = dModeBrig*percentage/100; // co to jest k?

      for(uint8_t i = 0; i < ledCount; ++i) {
        leds[i] = Rgb{(activeLeds<=i)?(uint8_t)0:(uint8_t)(dModeBrig-k),(activeLeds<=i)?(uint8_t)0:(uint8_t)k,0};
      }    
                     
    } else {
      uint8_t k = actBrig*percentage/100; // tutaj kolejne K, tylko inne? Naprawdę nie można nazwać zmiennej tak aby coś znaczyła?

      for(uint8_t i = 0; i < ledCount; ++i) {
        leds[i] = Rgb{(activeLeds<=i)?(uint8_t)0:(uint8_t)(actBrig-k),(activeLeds<=i)?(uint8_t)0:(uint8_t)k,0}; // <-- niech ktoś to rozszyfruje i poprawi
      }
    }
    leds.show();
  }
}

/************************************************************************************/

static void buzzerTask(void*) {
  
  pinMode(buzzerPin, OUTPUT);

  while(1) {
    
    int timeCopy = buzzerDelTime; // do czego służy timeCopy? nie da się tutaj użyć buzzerDelTime

    if(timeCopy > 0) {
      
      digitalWrite(buzzerPin, 1);
      delay(100);
      digitalWrite(buzzerPin, 0);
      delay(timeCopy);
    }
  }
}

/************************************************************************************/

void setup() {

  Serial.begin(115200);
  Serial.printf("Version: %d\n", LAST_BUILD_TIME);
  
  distanceQueue = xQueueCreate(5, sizeof(uint));
  secondQueue = xQueueCreate(5, sizeof(uint));


  pinMode(simpleLed, OUTPUT);
  digitalWrite(simpleLed, 1);

  wifi.connect();
  mqtt.connect(wifi.client);
  
  statusEntity.configure();
  distanceEntity.configure();
  secondDistEntity.configure();
  orientationEntity.configure();

  delay(1000);
  digitalWrite(simpleLed, 0);

  Ultrasonic sensor(TRIG, ECHO, SENSOR_PWM);
  Ultrasonic secondSensor(secondEcho);

  xTaskCreate(ledTask, "Ledy_Task", 4096, nullptr, 1, NULL);
  xTaskCreate(buzzerTask, "Buzzer_Task", 1024, nullptr, 6, NULL);
}

void loop() {}
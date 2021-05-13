#define INCLUDE_vTaskSuspend 1

#include "secrets.hpp"
#include "pinout.hpp"

#include <SmartLeds.h>
#include <ultrasonic.hpp>

#include <wifi.hh>
#include <mqtt.hh>

const ledc_channel_t SENSOR_PWM = LEDC_CHANNEL_0;
const ledc_channel_t ledPwmChannel = LEDC_CHANNEL_1;

myWifi wifi(ssid, password);
myMQTT mqtt(project_name, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_dir);

QueueHandle_t distanceQueue;

Entity statusEntity(EntityType::binarySensor, "AutoStatus", &mqtt);
Entity distanceEntity(EntityType::sensor, "Distance", &mqtt);

bool stan = false;
uint distance;

/************************************************************************************/

static void ledTask(void*) {
  SmartLed leds(LED_WS2812B, ledCount, ledPin, ledPwmChannel);
  uint lastDistance = 0;
  unsigned long noDiffTimer = millis();

  /************************************************/

  while(1) {
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
        }
        if(stan && percentage > 85) {

          statusEntity.update("OFF");
          printf("MQTT auto status OFF\n");
        }

        uint k = darkModeBrightness*percentage/100;

        for(uint i = 0; i < ledCount; ++i) {
          leds[i] = Rgb{(activeLeds<=i)?(uint8_t)0:(uint8_t)darkModeBrightness-k,(activeLeds<=i)?(uint8_t)0:(uint8_t)k,0};
        }

      }
      else {

        uint k = activeBrightness*percentage/100;

        for(uint i = 0; i < ledCount; ++i) {
          leds[i] = Rgb{(activeLeds<=i)?(uint8_t)0:(uint8_t)activeBrightness-k,(activeLeds<=i)?(uint8_t)0:(uint8_t)k,0};
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

  pinMode(GPIO_NUM_25, OUTPUT);
  digitalWrite(GPIO_NUM_25, 1);

  wifi.connect();
  mqtt.connect(wifi.client);
  
  statusEntity.configure();
  distanceEntity.configure();

  digitalWrite(GPIO_NUM_25, 0);

  while(1) {

    mqtt.loop();
    delay(330);
    distanceEntity.update(distance);
  }
}

/************************************************************************************/

void setup() {

  Serial.begin(115200);
  Serial.printf("Version: %d\n", LAST_BUILD_TIME);
  
  distanceQueue = xQueueCreate(5, sizeof(uint));
  xTaskCreate(ledTask, "Ledy_Task", 4096, nullptr, 3, NULL);
  xTaskCreate(buzzerTask, "Buzzer_Task", 4096, nullptr, 4, NULL);
  xTaskCreate(mqttTask, "Mqtt_Task", 4096, nullptr, 4, NULL);

  Ultrasonic sensor(TRIG, ECHO, SENSOR_PWM);
}

void loop() {}
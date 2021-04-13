#include <Arduino.h>
#include "secrets.hpp"
#include <wifi.hh>
#include <mqtt.hh>
#include "esp32sr04.h"
#include "LedRGB.h"

myWifi wifi(ssid, password);
myMQTT mqtt(project_name, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_dir, entity_name);

unsigned long lastMillis = 0;
bool stan = 0;



void setup() {
  
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  digitalWrite(2, 1);
  delay(100);
  digitalWrite(2, 0);

  delay(100);
  Serial.printf("Version: %d\n", LAST_BUILD_TIME);
  //wifi.connect();
  //mqtt.connect(wifi.client);
  //mqtt.configure();
  
  initSensors();
  init_RGB();
}

void loop() {
  
  //mqtt.loop();
  delay(10);

  show_RGB(espSr04.distance1_cm);


  if (millis() - lastMillis > 1000) {
    //mqtt.autoStatus(stan);
    Serial.println(stan);
    stan = !stan;
    lastMillis = millis();
  }
}

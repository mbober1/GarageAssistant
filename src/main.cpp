#include <Arduino.h>
#include "secrets.hpp"
#include <wifi.hh>
#include <mqtt.hh>
#include "esp32sr04.h"
#include <SmartLeds.h>

myWifi wifi(ssid, password);
myMQTT mqtt(project_name, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_dir, entity_name);
Esp32sr04 dupa(ECHO1_PIN, TRIG_PIN);

unsigned long lastMillis = 0;
bool stan = 0;

void IRAM_ATTR echoCallback() {
    if(digitalRead(dupa.echoPin)) {
      dupa.distance = (micros() - dupa.sensorTime)/58;
      if(dupa.distance > 200) dupa.distance = -1;
    } else dupa.sensorTime = micros();
}


void setup() {
  
  Serial.begin(115200);
  pinMode(2, OUTPUT);
  Serial.printf("Version: %d\n", LAST_BUILD_TIME);
  wifi.connect();
  //mqtt.connect(wifi.client);
  //mqtt.configure();
}

void loop() {
  
  mqtt.loop();


  if (millis() - lastMillis > 1000) {
    // mqtt.autoStatus(stan);
    Serial.println(stan);
    stan = !stan;
    lastMillis = millis();
  }
}

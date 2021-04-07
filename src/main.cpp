#include <Arduino.h>
#include "secrets.hpp"
#include <wifi.hh>
#include <mqtt.hh>

myWifi wifi(ssid, password);
myMQTT mqtt(project_name, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_dir, entity_name);

unsigned long lastMillis = 0;
bool stan = 0;



void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.printf("Version: %d\n", LAST_BUILD_TIME);
  wifi.connect();
  mqtt.connect(wifi.client);
  mqtt.configure();
}

void loop() {
  mqtt.loop();
  delay(10);

  if (millis() - lastMillis > 1000) {
    mqtt.autoStatus(stan);
    Serial.println(stan);
    stan = !stan;
    lastMillis = millis();
  }
}
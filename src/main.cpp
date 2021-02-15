#include <Arduino.h>
#include <MQTT.h>
#include <ESP8266WiFi.h>
// #include <ESPhttpUpdate.h>
#include "secrets.hpp"


WiFiClient wifi;
MQTTClient mqtt;

unsigned long lastMillis = 0;
bool stan = 0;
char buffer[100];
char buffer2[100];


void connect() {
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to WiFi!");

  Serial.printf("\nConnecting to %s", mqtt_server);
  mqtt.begin(mqtt_server, wifi);
  while (!mqtt.connect(project_name, mqtt_user, mqtt_pass)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Home Assistant!");
}

void configure() {
  sprintf(buffer, "homeassistant/binary_sensor/%s/car/config", mqtt_dir);
  sprintf(buffer2, "{\"name\": \"%s\", \"state_topic\": \"homeassistant/binary_sensor/%s/car/state\"}", entity_name, mqtt_dir);
  mqtt.publish(buffer, buffer2);
  Serial.println("Configured MQTT!");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.printf("Version: %d\n", LAST_BUILD_TIME);
  pinMode(LED_BUILTIN, OUTPUT);


  connect();
  configure();


  // t_httpUpdate_return ret = ESPhttpUpdate.update("192.168.0.2", 80, "/esp/update/arduino.php", LAST_BUILD_TIME);
  // switch(ret) {
  //   case HTTP_UPDATE_FAILED:
  //       Serial.println("[update] Update failed.");
  //       break;
  //   case HTTP_UPDATE_NO_UPDATES:
  //       Serial.println("[update] Update no Update.");
  //       break;
  //   case HTTP_UPDATE_OK:
  //       Serial.println("[update] Update ok."); // may not be called since we reboot the ESP
  //       break;
  // }

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());


}

void loop() {
  mqtt.loop();
  delay(10);

  if (millis() - lastMillis > 1000) {
    sprintf(buffer, "homeassistant/binary_sensor/%s/car/state", mqtt_dir);
    mqtt.publish(buffer, stan ? "ON" : "OFF");
    digitalWrite(LED_BUILTIN, stan);
    stan = !stan;
    lastMillis = millis();
  }
}
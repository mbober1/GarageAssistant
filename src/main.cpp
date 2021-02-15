#include <Arduino.h>
#include <MQTT.h>
#include <ESP8266WiFi.h>
#include "secrets.hpp"


WiFiClient wifi;
MQTTClient mqtt;

unsigned long lastMillis = 0;
bool stan = 0;
char buffer[100];
char buffer2[100];


void connect() {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to WiFi!");

  Serial.print("\nConnecting to Home Assistant");
  while (!mqtt.connect(project_name, mqtt_user, mqtt_pass)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nConnected to Home Assistant!");
}

void configure() {
  sprintf(buffer, "homeassistant/binary_sensor/%s/car/config", mqtt_dir);
  sprintf(buffer2, "{\"name\": \"%s\", \"state_topic\": \"homeassistant/binary_sensor/%s/car/state\"}", entity_name, mqtt_dir);
  mqtt.publish(buffer, buffer2);
  Serial.println("Configured!");
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  WiFi.begin(ssid, password);
  mqtt.begin(mqtt_server, wifi);

  connect();
  configure();
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
#include <Arduino.h>
#include <MQTTClient.h>
#include <WiFi.h>
#include "secrets.hpp"
#include <WiFiClientSecure.h>
#include <time.h>

WiFiClient wifi;
MQTTClient mqtt;

#define LED_BUILTIN 2

unsigned long lastMillis = 0;
bool stan = 0;
char buffer[100];
char buffer2[100];

  void connect() {
  Serial.printf("Connecting to %s ", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConnected to WiFi! My IP: ");
  Serial.println(WiFi.localIP());

  configTime(3600, 0, "pool.ntp.org");  
  time_t tnow = time(nullptr);
  struct tm *timeinfo;

  time(&tnow);
  timeinfo = localtime(&tnow);
  Serial.printf("%d:%d:%d %d/%d/%d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year);

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
  pinMode(LED_BUILTIN, OUTPUT);
  delay(1000);
  Serial.printf("Version: %d\n", LAST_BUILD_TIME);


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
    Serial.println(stan);
    stan = !stan;
    lastMillis = millis();
  }
}
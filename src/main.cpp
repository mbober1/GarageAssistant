#include <Arduino.h>
#include <SmartLeds.h>
// #include "secrets.hpp"
//#include <wifi.hh>
//#include <mqtt.hh>
// #include "esp32sr04.h"
// #include "LedRGB.h"

//myWifi wifi(ssid, password);
//myMQTT mqtt(project_name, mqtt_server, mqtt_port, mqtt_user, mqtt_pass, mqtt_dir, entity_name);

 unsigned long lastMillis = 0;
 bool stan = 0;
 int distance = 150;
  
SmartLed test1(LED_WS2812, 30, 2, 1);

static void ledTask(void*) {
  Serial.println("Zaczalem taska");

  //test1[0] = Rgb{0,0,150};
  //test1.show();
  Serial.println("Dane poszly");
    Serial.println("Dupa");
    
    if(distance > 200)
    {
      Serial.println("Blad");
    }
    else if(distance < 2)
    {
      for(int i=0; i<30; i++)
      {
         test1[i] = Rgb{250,0,0};
         test1.show();
      }
    }
    else
    {
      int k = (int) distance*1.25;
      for(int i=0; i<30; i++)
      {
         test1[i] = Rgb{255-k, k, 0};
         test1.show();
      }
    }
    
    test1.show();

    delay(1000);
  
 Serial.println("End task");
  vTaskDelete(NULL);
}



void setup() {
  Serial.begin(115200);
 // delay(1000);
  //Serial.printf("Version: %d\n", LAST_BUILD_TIME);
  // wifi.connect();
  // mqtt.connect(wifi.client);
  // mqtt.configure();
 xTaskCreate(ledTask, "Ledy_Task", 4096, nullptr, 5, NULL); 
  // initSensors();
  //init_RGB();
}




void loop() {
  // mqtt.loop();
  

  Serial.println("xD");
  delay(1000);
  // show_RGB(20);
  // int czujnik1 = espSr04.distance1_cm;
  // int czujnik2 = espSr04.distance2_cm;
  
 /* test1.show();
  if (millis() - lastMillis > 1000) {
    // mqtt.autoStatus(stan);
    Serial.println(stan);
    stan = !stan;
    lastMillis = millis();
  }*/
}

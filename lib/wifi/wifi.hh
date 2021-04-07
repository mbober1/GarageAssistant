#include <WiFiClientSecure.h>


class myWifi : public WiFiSTAClass {

    const char* ssid;
    const char* password;

public:
    myWifi(const char* ssid, const char* password);
    void connect();
    WiFiClient client;
};

myWifi::myWifi(const char* ssid, const char* password) : ssid(ssid), password(password) {}

void myWifi::connect() {
  Serial.printf("Connecting to %s ", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.print("\nConnected to WiFi! My IP: ");
  Serial.println(WiFi.localIP());

//   configTime(3600, 0, "0.pool.ntp.org", "1.pool.ntp.org", "2.pool.ntp.org");  
//   time_t tnow = time(nullptr);
//   struct tm *timeinfo = {};

//   time(&tnow);
//   timeinfo = localtime(&tnow);
//   printf("Time: %d:%d:%d %d/%d/%d\n", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, timeinfo->tm_mday, timeinfo->tm_mon, timeinfo->tm_year);
}
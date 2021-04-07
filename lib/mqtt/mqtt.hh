#include <MQTTClient.h>


class myMQTT : public MQTTClient {
    const char* project_name;
    const char* mqtt_server;
    const char* mqtt_user;
    const char* mqtt_pass;
    const char* mqtt_dir;
    const char* entity_name;
    unsigned int port;
    

public:
    myMQTT(const char* project_name, const char* mqtt_server,  unsigned int port, const char* mqtt_user, const char* mqtt_pass, const char* mqtt_dir, const char* entity_name);
    void configure();
    void connect(Client &_client);
    
    void autoStatus(bool status);
};

myMQTT::myMQTT(const char* project_name, const char* mqtt_server, unsigned int port, const char* mqtt_user, const char* mqtt_pass, const char* mqtt_dir, const char* entity_name) : 
project_name(project_name), 
mqtt_server(mqtt_server), 
mqtt_user(mqtt_user), 
mqtt_pass(mqtt_pass), 
mqtt_dir(mqtt_dir), 
entity_name(entity_name), 
port(port) {}


void myMQTT::configure() {
  char buffer[100];
  char buffer2[100];

  sprintf(buffer, "homeassistant/binary_sensor/%s/car/config", this->mqtt_dir);
  sprintf(buffer2, "{\"name\": \"%s\", \"state_topic\": \"homeassistant/binary_sensor/%s/car/state\"}", this->entity_name, this->mqtt_dir);
  publish(buffer, buffer2);
  printf("Configured MQTT!\n");
}

  
void myMQTT::connect(Client &_client) {
  printf("\nConnecting to %s", this->mqtt_server);
  MQTTClient::begin(this->mqtt_server, 6969, _client);
  while (!MQTTClient::connect(this->project_name, this->mqtt_user, this->mqtt_pass)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Home Assistant!\n");
}

void myMQTT::autoStatus(bool status) {
  char buffer[100];
  sprintf(buffer, "homeassistant/binary_sensor/%s/car/state", this->mqtt_dir);
  publish(buffer, status ? "ON" : "OFF");
}
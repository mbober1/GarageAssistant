#include <MQTTClient.h>
#include <string>

enum class EntityType {
  sensor,
  binarySensor
};


class myMQTT : public MQTTClient {
    const char* project_name;
    const char* mqtt_server;
    const char* mqtt_user;
    const char* mqtt_pass;
    const char* mqtt_dir;
    unsigned int port;
    
public:
    myMQTT(const char* project_name, const char* mqtt_server,  unsigned int port, const char* mqtt_user, const char* mqtt_pass, const char* mqtt_dir);
    void connect(Client &_client);
    const char* getMqttDir();
};


myMQTT::myMQTT(const char* project_name, const char* mqtt_server, unsigned int port, const char* mqtt_user, const char* mqtt_pass, const char* mqtt_dir) : 
project_name(project_name), 
mqtt_server(mqtt_server), 
mqtt_user(mqtt_user), 
mqtt_pass(mqtt_pass), 
mqtt_dir(mqtt_dir), 
port(port) {}

const char* myMQTT::getMqttDir() { return this->mqtt_dir; }


  
void myMQTT::connect(Client &client) {
  printf("\nConnecting to %s\n", this->mqtt_server);
  MQTTClient::begin(this->mqtt_server, mqtt_port, client);
  while (!MQTTClient::connect(this->project_name, this->mqtt_user, this->mqtt_pass)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Home Assistant!\n");
}


class Entity {
  EntityType type;
  String name;
  myMQTT *mqtt;

  String getTypeName();

  public:
    Entity(EntityType type, String name, myMQTT *mqtt);
    void configure();
    void update(String data);
    void update(int data);
    String getTopic();
    String getPayload();
};


Entity::Entity(EntityType type, String name, myMQTT *mqtt) :
type(type),
name(name),
mqtt(mqtt) {}


String Entity::getTopic() {
  String topic("homeassistant/");
  topic += this->getTypeName();
  topic += "/";
  topic += mqtt->getMqttDir();
  topic += "/car/";
  return topic;
}


String Entity::getPayload() {
  String payload("{\"name\": \"");
  payload += this->name;
  payload += "\", \"state_topic\": \"homeassistant/";
  payload += this->getTypeName();
  payload += "/";
  payload += mqtt->getMqttDir();
  payload += "/car/";
  payload += "value";
  payload += "\"}";
  return payload;
}


void Entity::configure() {
  String topic = this->getTopic();
  String payload = this->getPayload();
  topic += "config";

  mqtt->publish(topic, payload);
  printf("Configured MQTT %s!\n", this->name.c_str());
}

void Entity::update(String data) {
  String topic = this->getTopic();
  topic += "value";

  this->mqtt->publish(topic, data);
}

void Entity::update(int data) {
  String topic = this->getTopic();
  topic += "value";

  this->mqtt->publish(topic, String(data));
}




String Entity::getTypeName() {
  switch (this->type)
  {
  case EntityType::sensor:
    return String("sensor");
    break;

  case EntityType::binarySensor:
    return String("binary_sensor");
    break;
  
  default:
    printf("Unkown mqtt type\n");
    return String();
    break;
  }
}




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
  static String addParamether(String data);


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


String Entity::addParamether(String data) {
  String tmp("/");
  tmp += data;
  return tmp;
}



String Entity::getTopic() {
  String topic("homeassistant");
  topic += Entity::addParamether(this->getTypeName());
  topic += Entity::addParamether(mqtt->getMqttDir());
  topic += Entity::addParamether("rak");
  return topic;
}


String Entity::getPayload() {
  String payload("{\"name\": \"");
  payload += this->name;
  payload += "\", \"state_topic\": \"homeassistant";
  payload += Entity::addParamether(this->getTypeName());
  payload += Entity::addParamether(mqtt->getMqttDir());
  payload += Entity::addParamether("rak");
  payload += Entity::addParamether("value");
  payload += "\"}";
  return payload;
}


void Entity::configure() {
  String topic = this->getTopic();
  String payload = this->getPayload();
  topic += Entity::addParamether("config");

  mqtt->publish(topic, payload);
  printf("Configured MQTT %s!\n", this->name.c_str());
}

void Entity::update(String data) {
  String topic = this->getTopic();
  topic += Entity::addParamether("value");

  this->mqtt->publish(topic, data);
}

void Entity::update(int data) {
  String topic = this->getTopic();
  topic += Entity::addParamether("value");

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




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
    void configure(String type, String entity_name, String data_name, String topic);
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

void myMQTT::configure(String type, String entity_name, String data_name, String topic) {
  topic += "config";

  String payload("{\"name\": \"");
  payload += entity_name;
  payload += "\", \"state_topic\": \"homeassistant/";
  payload += type;
  payload += "/";
  payload += this->mqtt_dir;
  payload += "/car/";
  payload += data_name;
  payload += "\"}";
  
  publish(topic, payload);
  printf("Configured MQTT %s->%s!\n", entity_name.c_str(), data_name.c_str());
}


  
void myMQTT::connect(Client &client) {
  printf("\nConnecting to %s\n", this->mqtt_server);
  MQTTClient::begin(this->mqtt_server, 6969, client);
  while (!MQTTClient::connect(this->project_name, this->mqtt_user, this->mqtt_pass)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Home Assistant!\n");
}


class Entity {
  EntityType type;
  String name;
  String valueDir;
  myMQTT *mqtt;

  String getTypeName(EntityType type);

  public:
    Entity(EntityType type, String name, String valueDir, myMQTT *mqtt);
    void configure();
    void update(String data);
    void update(int data);
    String getTopic();

};

Entity::Entity(EntityType type, String name, String valueDir, myMQTT *mqtt) :
type(type),
name(name),
valueDir(valueDir),
mqtt(mqtt) {}

String Entity::getTopic() {
  String topic("homeassistant/");
  topic += this->getTypeName(this->type);
  topic += "/";
  topic += mqtt->getMqttDir();
  topic += "/car/";
  return topic;
}


void Entity::configure() {
  this->mqtt->configure(this->getTypeName(this->type), this->name, this->valueDir, this->getTopic());
}

void Entity::update(String data) {
  String topic = this->getTopic();
  topic += this->valueDir;

  this->mqtt->publish(topic, data);
}

void Entity::update(int data) {
  String topic = this->getTopic();
  topic += this->valueDir;

  this->mqtt->publish(topic, String(data));
}




String Entity::getTypeName(EntityType type) {
  switch (type)
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




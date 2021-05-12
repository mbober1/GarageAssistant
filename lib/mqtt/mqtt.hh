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
    void configure(std::string type, std::string entity_name, std::string data_name, std::string topic);
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

void myMQTT::configure(std::string type, std::string entity_name, std::string data_name, std::string topic) {
  topic.append("config");

  std::string payload("{\"name\": \"");
  payload.append(entity_name);
  payload.append("\", \"state_topic\": \"homeassistant/");
  payload.append(type);
  payload.append("/");
  payload.append(this->mqtt_dir);
  payload.append("/car/");
  payload.append(data_name);
  payload.append("\"}");
  
  publish(topic.c_str(), payload.c_str());
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
  std::string name;
  std::string valueDir;
  myMQTT *mqtt;

  std::string getTypeName(EntityType type);

  public:
    Entity(EntityType type, std::string name, std::string valueDir, myMQTT *mqtt);
    void configure();
    void update(std::string data);
    void update(int data);
    std::string getTopic();

};

Entity::Entity(EntityType type, std::string name, std::string valueDir, myMQTT *mqtt) :
type(type),
name(name),
valueDir(valueDir),
mqtt(mqtt) {}

std::string Entity::getTopic() {
  std::string topic("homeassistant/");
  topic += this->getTypeName(this->type);
  topic.append("/");
  topic.append(mqtt->getMqttDir());
  topic.append("/car/");
  return topic;
}


void Entity::configure() {
  this->mqtt->configure(this->getTypeName(this->type), this->name, this->valueDir, this->getTopic());
}

void Entity::update(std::string data) {
  std::string topic = this->getTopic();
  topic.append(this->valueDir);

  this->mqtt->publish(topic.c_str(), data.c_str());
}

void Entity::update(int data) {
  std::string topic = this->getTopic();
  topic.append(this->valueDir);

  this->mqtt->publish(topic.c_str(), String(data));
}




std::string Entity::getTypeName(EntityType type) {
  switch (type)
  {
  case EntityType::sensor:
    return std::string("sensor");
    break;

  case EntityType::binarySensor:
    return std::string("binary_sensor");
    break;
  
  default:
    printf("Unkown mqtt type\n");
    return std::string();
    break;
  }
}




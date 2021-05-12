#include <MQTTClient.h>


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
    // const char* entity_name;
    unsigned int port;
    
    std::string getTypeName(EntityType type);

public:
    myMQTT(const char* project_name, const char* mqtt_server,  unsigned int port, const char* mqtt_user, const char* mqtt_pass, const char* mqtt_dir);
    void configure(EntityType type, std::string entity_name, std::string data_name);
    void connect(Client &_client);
    
    void autoStatus(bool status);
};

myMQTT::myMQTT(const char* project_name, const char* mqtt_server, unsigned int port, const char* mqtt_user, const char* mqtt_pass, const char* mqtt_dir) : 
project_name(project_name), 
mqtt_server(mqtt_server), 
mqtt_user(mqtt_user), 
mqtt_pass(mqtt_pass), 
mqtt_dir(mqtt_dir), 
port(port) {}


std::string myMQTT::getTypeName(EntityType type) {
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

void myMQTT::configure(EntityType type, std::string entity_name, std::string data_name) {
  std::string topic("homeassistant/");
  topic += this->getTypeName(type);
  topic.append("/");
  topic += this->mqtt_dir;
  topic.append("/car/config");

  std::string payload("{\"name\": \"");
  payload.append(entity_name);
  payload.append("\", \"state_topic\": \"homeassistant/");
  payload += this->getTypeName(type);
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

void myMQTT::autoStatus(bool status) {
  char buffer[100];
  sprintf(buffer, "homeassistant/binary_sensor/%s/car/state", this->mqtt_dir);
  publish(buffer, status ? "ON" : "OFF");
}

// class Entity : public myMQTT {
//   EntityType type;
//   std::string name;
//   std::string valueDir;

//   public:
//     Entity(EntityType type, std::string name, std::string valueDir);
// };

// Entity::Entity(EntityType type, std::string name, std::string valueDir) :
//   type(type),
//   name(name),
//   valueDir(valueDir) {}


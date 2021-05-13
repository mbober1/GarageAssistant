#include <mqtt.hpp>

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
  MQTTClient::begin(this->mqtt_server, this->port, client);
  while (!MQTTClient::connect(this->project_name, this->mqtt_user, this->mqtt_pass)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Home Assistant!\n");
}






Entity::Entity(EntityType type, String name, myMQTT *mqtt) :
type(type),
name(name),
mqtt(mqtt) {}


String Entity::addParamether(String data) {
  String tmp("/");
  tmp += data;
  return tmp;
}


String Entity::addVariable(String name, String data, String separator = "") {
  String tmp("\"");
  tmp += name;
  tmp += "\": \"";
  tmp += data;
  tmp += "\"";
  tmp += separator;

  return tmp;
}



String Entity::getTopic() {
  String topic("homeassistant");
  topic += Entity::addParamether(this->getTypeName());
  topic += Entity::addParamether(this->name);
  return topic;
}


String Entity::getPayload() {
  String payload("{");
  String path("homeassistant");
  path += Entity::addParamether(this->getTypeName());
  path += Entity::addParamether(this->name);
  path += Entity::addParamether("value");

  payload += Entity::addVariable("unique_id", this->name, ", ");
  payload += Entity::addVariable("state_topic", path);
  payload += "}";
  return payload;
}


void Entity::configure() {
  String topic = this->getTopic();
  String payload = this->getPayload();
  topic += Entity::addParamether("config");

  mqtt->publish(topic, payload);
  printf("Configured MQTT %s!\n", this->name.c_str());
}


void Entity::update(String newData) {
  String topic = this->getTopic();
  topic += Entity::addParamether("value");

  if(this->data != newData) {
    this->data = newData;
    this->mqtt->publish(topic, this->data);
  }
}


void Entity::update(int newData) {
  Entity::update(String(newData));
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
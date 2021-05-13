#pragma once
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




class Entity {
  EntityType type;
  String name, data;
  myMQTT *mqtt;

  String getTypeName();
  static String addParamether(String data);
  static String addVariable(String name, String data, String separator);


public:
  Entity(EntityType type, String name, myMQTT *mqtt);
  void configure();
  void update(String data);
  void update(int data);
  String getTopic();
  String getPayload();
};







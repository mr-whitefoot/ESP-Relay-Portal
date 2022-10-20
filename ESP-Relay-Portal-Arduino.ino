#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EspMQTTClient.h>
#include <ArduinoJson.h>
#include <TimerMs.h>
#include <LittleFS.h>
#include <GyverPortal.h>
#include <EEManager.h>
#include "ESPRelay.h"


String version = "1.6.2";

struct Data {
  //Data
  char label[32] = "Relay";
  char device_name[32] = "relay";
  bool relayInvertMode = false;
  bool factoryReset = true;
  int wifiConnectTry = 0;
  bool wifiAP;
  bool relaySaveStatus = false;
  bool state;
  
  // WiFi
  char ssid[32];
  char password[32];

  //MQTT
  char mqttServerIp[32];
  short mqttServerPort = 1883;
  char mqttUsername[32];
  char mqttPassword[32];
  //Delay before send message in seconds
  int status_delay = 10;
  int avaible_delay = 60;
  
  //MQTT Topic
  char discoveryTopic[100] = "homeassistant/switch/relay/config";
  char commandTopic[100]   = "homeassistant/switch/relay/set";
  char avaibleTopic[100]   = "homeassistant/switch/relay/avaible";
  char stateTopic[100]     = "homeassistant/switch/relay/state";
};

#define WIFIAPTIMER 180000
Data data;
GyverPortal portal;
EEManager memory(data);
GPlog glog("log");

struct Form{
  String root = "/";
  String log = "/log";
  String config = "/config";
  String preferences = "/config/preferences";
  String WiFiConfig ="/config/wifi_config";
  String mqttConfig = "/config/mqtt_config";
  String mqttTopic = "/config/mqtt_config/topic";
  String factoryReset = "/config/factory_reset";
  String firmwareUpgrade = "/ota_update";
};

Form form;

TimerMs MessageTimer;
TimerMs ServiceMessageTimer;
TimerMs WiFiApTimer;
EspMQTTClient client;
ESPRelay Relay1;
bool resetAllow;

void publishRelay();
void SendDiscoveryMessage();
void SendAvailableMessage();
void publishLed();
void wifiAp();
void wifiConnect();
void mqttPublish();
void portalBuild();
void portalAction();
void portalCheck();
void factoryReset();
void ChangeRelayState();
void println(String text);
void print(String text);



#include "webface.h"
#include "mqtt.h"
#include "function.h"


void setup() {
  startup();
}


void loop(){
  ArduinoOTA.handle();
  client.loop();
  mqttPublish();
  portal.tick();
}





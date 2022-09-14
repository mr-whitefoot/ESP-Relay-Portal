#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <EspMQTTClient.h>
#include <ArduinoJson.h>
#include <TimerMs.h>
#include <GyverPortal.h>
#include <EEManager.h>
#include "ESPRelay.h"

struct Data {
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
  //Data
  char label[32] = "Relay";
  char device_name[32] = "relay";
  bool factoryReset = true;
  int wifiConnectTry = 0;
  bool wifiAP;

  char discoveryTopic[100] = "homeassistant/switch/relay/config";
  char commandTopic[100]   = "homeassistant/switch/relay/set";
  char avaibleTopic[100]   = "homeassistant/switch/relay/avaible";
  char stateTopic[100]     = "homeassistant/switch/relay/state";
};

Data data;
GyverPortal portal;
EEManager memory(data);

struct Form{
  String status = "/status";
  String config = "/config";
  String general = "/config/general";
  String WiFiConfig ="/config/wifi_config";
  String mqttConfig = "/config/mqtt_config";
  String mqttTopic = "/config/mqtt_config/topic";
  String factoryReset = "/config/factory_reset";
  String firmwareUpgrade = "/ota_update";
};

Form form;

TimerMs MessageTimer;
TimerMs ServiceMessageTimer;
EspMQTTClient client;
ESPRelay Relay1(0);
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



void setup() {
  delay(1000);
  Serial.begin(9600);
  Serial.println();
  Serial.println("Booting...");
  Serial.println("Initialize EEPROM");
  EEPROM.begin(sizeof(data) + 1); // +3 на ключ
  memory.begin(0, 'k');         // запускаем менеджер памяти
  
  // Connecting WiFi
  Serial.println(F("Initialize WiFi"));

  if (data.factoryReset == true || data.wifiAP == true ) wifiAp();
  
  // Enable OTA update
  ArduinoOTA.begin();

  if (data.factoryReset==false){
    if (data.factoryReset == true || data.ssid == "" ) wifiAp();
    else wifiConnect();

  //MQTT
  client.setWifiCredentials(data.ssid, data.password);
  client.setMqttServer(data.mqttServerIp, data.mqttUsername, data.mqttPassword, data.mqttServerPort );
  client.setMqttClientName(data.device_name);
  //Настройка максимальной длинны сообщения MQTT
  client.setMaxPacketSize(1000);

  //Таймеры
  MessageTimer.setTime(data.status_delay*1000);
  MessageTimer.start();
  ServiceMessageTimer.setTime(data.avaible_delay*1000);
  ServiceMessageTimer.start();

  // подключаем конструктор портала и запускаем
  portal.attachBuild(portalBuild);
  portal.disableAuth();
  portal.attach(portalAction);
  portal.start(data.device_name);
  portal.enableOTA();
  
  Serial.println("Boot complete");
  }

}

void wifiAp(){
  // создаём точку с именем Relay
  Serial.println(F("Create AP"));
  String ssid("RelayAP");
  WiFi.softAP(ssid);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  data.wifiConnectTry = 0;
  memory.updateNow();

  portal.attachBuild(portalBuild);
  portal.attach(portalAction);
  portal.start(data.device_name);    // запускаем портал
  while (portal.tick()) {   // портал работает
    
  }
}

void wifiConnect()
{
  WiFi.hostname(data.device_name);
  WiFi.begin(data.ssid, data.password);
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.println("Wifi connecting...");
    notConnectedCounter++;
    if (notConnectedCounter > 150)
    { // Reset board if not connected after 5s
      Serial.println("Resetting due to Wifi not connecting...");
      
      data.wifiConnectTry += 1;
      if(data.wifiConnectTry == 100);
        data.wifiAP = true;
      memory.updateNow();
      
      ESP.restart();
    }
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  Serial.print("Wifi connected, IP address: ");
  Serial.println(WiFi.localIP());
}


bool ToBool( String value){
  if ( (value == "true" )||
       (value == "True" )||
       (value == "TRUE" )){
        return true;
       };
  
  if ( value == "false" ||
       value == "False" ||
       value == "FALSE" ){
        return false;
       };
  
  return false;
}

void loop(){
  ArduinoOTA.handle();
  client.loop();
  mqttPublish();
  portal.tick();
}


void portalBuild(){
  String p;
  BUILD_BEGIN(p);
  add.THEME(GP_DARK);

// Страница конфигурации
if (portal.uri() == form.config.c_str()) {
    add.BUTTON_LINK(form.general.c_str(), "General"); add.BREAK();
    add.BUTTON_LINK(form.WiFiConfig.c_str(), "WiFi configuration"); add.BREAK();
    add.BUTTON_LINK(form.mqttConfig.c_str(), "MQTT configuration"); add.BREAK();
    add.BUTTON_LINK(form.factoryReset.c_str(), "Factory reset"); add.BREAK();
    add.BUTTON_LINK(form.firmwareUpgrade.c_str(), "Firmware upgrade"); add.BREAK();
    add.BUTTON_LINK("/", "Back");

  } else if (portal.uri() == form.general){
    add.FORM_BEGIN(form.general.c_str());
    add.LABEL("General");
    add.BLOCK_BEGIN();
    add.TEXT("label", "Label", data.label); add.BREAK();
    add.TEXT("device_name", "Device name", data.device_name);
    add.BLOCK_END();
    add.SUBMIT("Save");    add.BREAK();
    add.FORM_END();    add.FORM_END();
    add.BUTTON_LINK(form.config.c_str(), "Back");


    // страница конфигурации wifi 
  } else if (portal.uri() == form.WiFiConfig) {
    add.FORM_BEGIN(form.WiFiConfig.c_str());
    add.LABEL("WIFI");

    add.BLOCK_BEGIN();
    add.LABEL_MINI("WiFi status"); 
    if (WiFi.status() == WL_CONNECTED){
      add.LED_GREEN("WiFiLed", true); add.BREAK();
      add.LABEL_MINI("IP address"); add.LABEL_MINI(WiFi.localIP().toString().c_str()); add.BREAK();}
    else add.LED_GREEN("WiFiLed", false);
    add.BLOCK_END();

    add.BLOCK_BEGIN();
    add.TEXT("ssid", "SSID", data.ssid);
    add.BREAK();
    add.PASS("pass", "Password", data.password);
    add.BLOCK_END();
    

    add.SUBMIT("Save and reboot");    add.BREAK();
    add.BUTTON_LINK(form.config.c_str(), "Back");
    add.FORM_END();


    // страница конфигурации MQTT
  } else if (portal.uri() == form.mqttConfig) {
    add.FORM_BEGIN(form.mqttConfig.c_str());
    add.LABEL("MQTT Server");

    add.BLOCK_BEGIN();
    add.LABEL_MINI("MQTT status"); 
    if (client.isConnected()){ add.LED_GREEN("MQTT", true); add.BREAK();}
    else{ add.LED_GREEN("MQTT", false); add.BREAK();}
    add.BLOCK_END();

    add.BLOCK_BEGIN();
    add.TEXT("mqttServerIp", "Server", data.mqttServerIp); add.BREAK();
    add.NUMBER("mqttServerPort", "Port", data.mqttServerPort); add.BREAK();
    add.TEXT("mqttUsername", "Username", data.mqttUsername); add.BREAK();
    add.PASS("mqttPassword", "Password", data.mqttPassword); add.BREAK();
    add.BLOCK_END();

    add.LABEL("MQTT Message periods");
    add.BLOCK_BEGIN();
    add.NUMBER("avaible_delay", "Avaible", data.avaible_delay); add.BREAK();
    add.NUMBER("status_delay", "Message", data.status_delay); add.BREAK();
    add.BLOCK_END();

    add.SUBMIT("Save and reboot"); add.BREAK();
    add.BUTTON_LINK(form.mqttTopic.c_str(), "MQTT Topic"); add.BREAK();
    add.BUTTON_LINK(form.config.c_str(), "Back"); add.BREAK();
    add.FORM_END();
    
  // Страница конфигурации топиков mqtt
  } else if (portal.uri() == form.mqttTopic) {
    add.FORM_BEGIN(form.mqttTopic.c_str());
    add.TITLE("MQTT topics");
    add.BLOCK_BEGIN();
    add.LABEL("Discovery topic"); add.BREAK();
    add.TEXT("discoveryTopic", "Discovery topic", data.discoveryTopic); add.BREAK();
    add.LABEL("Command topic"); add.BREAK();
    add.TEXT("commandTopic", "Command topic", data.commandTopic); add.BREAK();
    add.LABEL("Avaible topic"); add.BREAK();
    add.TEXT("avaibleTopic", "Avaible topic", data.avaibleTopic); add.BREAK();
    add.LABEL("State topic"); add.BREAK();
    add.TEXT("stateTopic", "State topic", data.stateTopic); add.BREAK();
    add.BLOCK_END();

    add.SUBMIT("Save and reboot"); add.BREAK();
    add.BUTTON_LINK(form.mqttConfig.c_str(), "Back");
    add.FORM_END();   

    //Сброс до заводских настроек
  } else if (portal.uri() == form.factoryReset) {
    add.FORM_BEGIN(form.factoryReset.c_str());
    add.TITLE( "Factory reset" );
    add.LABEL_MINI("I'm really understand what I do"); 
    add.CHECK("resetAllow", resetAllow);
    add.SUBMIT("Factory reset"); add.BREAK();
    add.BUTTON_LINK(form.config.c_str(), "Back");  add.BREAK();
    add.FORM_END();
    
    // главная страница, корень, "/"
  } else {
    add.FORM_BEGIN(form.status.c_str());
    add.BLOCK_BEGIN();
    add.LABEL( data.label ); add.SWITCH("switch", Relay1.GetState()); add.BREAK();
    add.BLOCK_END();
    add.BUTTON_LINK(form.config.c_str(), "Configuration"); add.BREAK();
    add.FORM_END();
  }

  BUILD_END();
}


void portalAction(){
  Serial.println("Portal Action");
  portalCheck();
    
  if (portal.click()){
    Serial.println("Portal click");
   
    if (portal.click("switch")){
      Relay1.SetState( portal.getCheck("switch") );
      publishRelay();
    }
  }
}

void portalCheck(){

  Serial.print("Portal form");
  Serial.println(portal.form());
  if (portal.form()) {
    Serial.println("Portal form handle");
    if (portal.form(form.WiFiConfig)) {
      portal.copyStr("ssid", data.ssid);
      portal.copyStr("pass", data.password);
      data.factoryReset = false;
      data.wifiAP = false;
      memory.updateNow();
      ESP.restart();
    } else if(portal.form(form.factoryReset)){
      Serial.println("Factory reset"); 
      if(portal.getCheck("resetAllow"));
        factoryReset();
    } else if(portal.form(form.general)){
      portal.copyStr("label", data.label);
      portal.copyStr("device_name", data.device_name);   
      memory.updateNow();
      
    } else if(portal.form(form.mqttConfig)){
      portal.copyStr("mqttServerIp", data.mqttServerIp);
      data.mqttServerPort = portal.getInt("mqttServerPort");
      portal.copyStr("mqttUsername", data.mqttUsername);
      portal.copyStr("mqttPassword", data.mqttPassword);
      data.avaible_delay = portal.getInt("avaible_delay");
      data.status_delay = portal.getInt("status_delay");
      memory.updateNow();
      ESP.restart();

//Топики
    } else if(portal.form(form.mqttTopic)){
      portal.copyStr("discoveryTopic", data.discoveryTopic);
      portal.copyStr("commandTopic", data.commandTopic);
      portal.copyStr("avaibleTopic", data.avaibleTopic);
      portal.copyStr("stateTopic", data.stateTopic);
      memory.updateNow();
      ESP.restart();    
    }
  }
}

void factoryReset(){
  Serial.println("Factory reset"); 
  memory.reset();
  data.factoryReset = true;
  memory.updateNow();
  ESP.restart();
}

void onConnectionEstablished() {
  Serial.println("MQTT server is connected");
  SendDiscoveryMessage();
  SendAvailableMessage();

  client.subscribe(data.commandTopic, [] (const String &payload)  {
    Serial.println("MQTT received command topic"); 
    Relay1.SetState( ToBool(payload));
    publishRelay();
  });
}


void publishRelay() {
  Serial.println("MQTT publish status");

  DynamicJsonDocument doc(1024);
  char buffer[256];
  doc["switch"] = Relay1.GetState();
  serializeJson(doc, buffer);
  client.publish(data.stateTopic, buffer, false);
}

void SendDiscoveryMessage( ){
  Serial.println("MQTT publish discovery message");
  DynamicJsonDocument doc(2048);
  char buffer[1024];

  String device_name = data.device_name;
  
  doc["name"]         = data.label;
  doc["uniq_id"]      = "ESP_"+device_name;
  doc["object_id"]    = "ESP_"+device_name;
  doc["avty_t"]       = data.avaibleTopic;
  doc["pl_avail"]     = "online";
  doc["pl_not_avail"] = "offline";
  doc["stat_t"]       = data.stateTopic;
  doc["stat_on"]      = true;
  doc["stat_off"]     = false;
  doc["cmd_t"]        = data.commandTopic;
  doc["pl_on"]        = true;
  doc["pl_off"]       = false;
  doc["dev_cla"]      = "switch";
  doc["val_tpl"]      = "{{ value_json.switch|default(false) }}";

  JsonObject device = doc.createNestedObject("device");
  device["name"] = "ESP_Device";
  device["model"] = "ESP_" + device_name;
  device["manufacturer"] = "whitefoot_company";
  JsonArray identifiers = device.createNestedArray("identifiers");
  identifiers.add("ESP_" + device_name);

  serializeJson(doc, buffer);
  client.publish(data.discoveryTopic, buffer, true);
}

void SendAvailableMessage(){
  Serial.println("MQTT publish avaible message");
  client.publish(data.avaibleTopic, "online", false);
}

void mqttPublish() {
  if (client.isConnected() && MessageTimer.tick()) {
    publishRelay();
  }

  if (client.isConnected() && ServiceMessageTimer.tick()) {
    SendAvailableMessage();
  }
}

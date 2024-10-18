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

void mqttStart(){
  println("Starting MQTT"); 

  #ifdef DEBUG_MQTT
    mqttClient.enableDebuggingMessages();
  #endif  

  mqttClient.setMqttServer( db[mqtt::serverIp].c_str(), 
                            db[mqtt::username].c_str(), 
                            db[mqtt::password1].c_str(),
                            db[mqtt::serverPort] 
                           );
  mqttClient.setMqttClientName(db[keys::deviceName].c_str());
  //Setup max lingth of message MQTT
  mqttClient.setMaxPacketSize(1000);

  // MQTT timers
  println("Starting MQTT timers");
  MessageTimer.setTime(db[mqtt::status_delay].toInt()*1000);
  MessageTimer.start();
  ServiceMessageTimer.setTime(db[mqtt::avaible_delay].toInt()*1000);
  ServiceMessageTimer.start();
}


void onConnectionEstablished() {
  println("MQTT server is connected");
  SendDiscoveryMessage();
  SendAvailableMessage("online");

  mqttClient.subscribe(db[mqtt::commandTopic], [] (const String &payload)  {
    println("MQTT received command topic"); 
    Relay1.SetState( ToBool(payload));
  });
}

void publishRelay() {
  if (!mqttClient.isConnected()){
    return;
  };
  println("MQTT publish status");

  DynamicJsonDocument doc(256);
  char buffer[256];
  doc["switch"] = Relay1.GetState();
  doc["WiFiRSSI"] = WiFi.RSSI(); 
  doc["IPAddress"] = WiFi.localIP().toString();

  serializeJson(doc, buffer);
  mqttClient.publish(db[mqtt::stateTopic], buffer, false);
}

void SendDiscoveryMessage( ){
  println("MQTT publish discovery message");
  DynamicJsonDocument doc(1024);
  char buffer[1024];

  String device_name = db[keys::deviceName];
  uint32_t chipId = ESP.getChipId();

  doc["name"]         = device_name;
  doc["uniq_id"]      = chipId;
  doc["object_id"]    = "ESP_"+device_name+"_"+WiFi.macAddress();
  doc["ip"]           = WiFi.localIP().toString();
  doc["mac"]          = WiFi.macAddress();
  doc["avty_t"]       = db[mqtt::avaibleTopic];
  doc["pl_avail"]     = "online";
  doc["pl_not_avail"] = "offline";
  doc["stat_t"]       = db[mqtt::stateTopic];
  doc["stat_on"]      = true;
  doc["stat_off"]     = false;
  doc["cmd_t"]        = db[mqtt::commandTopic];
  doc["pl_on"]        = true;
  doc["pl_off"]       = false;
  doc["dev_cla"]      = "switch";
  doc["val_tpl"]      = "{{ value_json.switch|default(false) }}";

  JsonObject device = doc.createNestedObject("device");
  device["name"] = device_name;
  device["model"] = "ESP_" + device_name + "_hw1.0";
  device["configuration_url"] = "http://"+WiFi.localIP().toString();
  device["manufacturer"] = "WhiteFoot company";
  device["sw_version"]   = sw_version;

  //JsonArray connections = device.createNestedArray("connections");
  //connections.add("ip,"+ WiFi.localIP().toString());
  //connections.add("mac,"+ WiFi.macAddress());

  JsonArray identifiers = device.createNestedArray("identifiers");
  identifiers.add(WiFi.macAddress());

  serializeJson(doc, buffer);
  mqttClient.publish(db[mqtt::discoveryTopic], buffer, true);
}

void SendAvailableMessage(const String &mode = "online"){
  println("MQTT publish available message");
  mqttClient.publish(db[mqtt::avaibleTopic], mode, false);
}

void mqttPublish() {
  if (mqttClient.isConnected() && MessageTimer.tick()) {
    publishRelay();
  }

  if (mqttClient.isConnected() && ServiceMessageTimer.tick()) {
    SendAvailableMessage();
  }
}
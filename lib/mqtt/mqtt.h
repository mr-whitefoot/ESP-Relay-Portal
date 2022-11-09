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

void onConnectionEstablished() {
  println("MQTT server is connected");
  SendDiscoveryMessage();
  SendAvailableMessage("online");

  mqttClient.subscribe(data.commandTopic, [] (const String &payload)  {
    println("MQTT received command topic"); 
    Relay1.SetState( ToBool(payload));
  });
}

void publishRelay() {
  println("MQTT publish status");

  DynamicJsonDocument doc(256);
  char buffer[256];
  doc["switch"] = Relay1.GetState();
  doc["IPAddress"] = WiFi.localIP().toString();
  doc["WiFiRSSI"] = WiFi.RSSI(); 
  serializeJson(doc, buffer);
  mqttClient.publish(data.stateTopic, buffer, false);
}

void SendDiscoveryMessage( ){
  println("MQTT publish discovery message");
  DynamicJsonDocument doc(1024);
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
  device["name"] = data.label;
  device["model"] = "ESP_" + device_name;
  device["manufacturer"] = "whitefoot_company";
  JsonArray identifiers = device.createNestedArray("identifiers");
  identifiers.add("ESP_" + device_name);

  serializeJson(doc, buffer);
  mqttClient.publish(data.discoveryTopic, buffer, true);
}

void SendAvailableMessage(const String &mode = "online"){
  println("MQTT publish avaible message");
  mqttClient.publish(data.avaibleTopic, mode, false);
}

void mqttPublish() {
  if (mqttClient.isConnected() && MessageTimer.tick()) {
    publishRelay();
  }

  if (mqttClient.isConnected() && ServiceMessageTimer.tick()) {
    SendAvailableMessage();
  }
}
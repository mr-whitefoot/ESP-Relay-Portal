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
  Serial.println("MQTT server is connected");
  SendDiscoveryMessage();
  SendAvailableMessage();

  client.subscribe(data.commandTopic, [] (const String &payload)  {
    Serial.println("MQTT received command topic"); 
    Relay1.SetState( ToBool(payload));
  });
}


void publishRelay() {
  Serial.println("MQTT publish status");

  DynamicJsonDocument doc(1024);
  char buffer[256];
  doc["switch"] = Relay1.GetState();
  doc["IPAddress"] = WiFi.localIP().toString();
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
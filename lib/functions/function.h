void println(const String& text){
  Serial.println(text);
  glog.println(text);
}

void print(const String& text){
  Serial.print(text);
  glog.print(text);
}

void wifiApStaTimerHandler(){
  WiFi.mode(WIFI_STA);
}

void mqttStart(){
  println("Starting MQTT"); 
  mqttClient.setMqttServer(data.mqttServerIp, data.mqttUsername, data.mqttPassword, data.mqttServerPort );
  mqttClient.setMqttClientName(data.device_name);
    //Setup max lingth of message MQTT
  mqttClient.setMaxPacketSize(1000);
}

void startup(){
  Serial.begin(9600);
  //Log
  glog.start(1000);

  println("-------------------------------");
  println("Booting...");
  println("Initialize EEPROM");
  EEPROM.begin(sizeof(data) + 1); // +1 key
  memory.begin(0, 'k');

  //Relay
  println("Initialize relay");
  Relay1.SetPin(RELAY_PIN);
  Relay1.SetInvertMode(data.relayInvertMode);
  Relay1.ChangeStateCallback(ChangeRelayState);
  if(data.relaySaveStatus){ 
      println("Restore relay state");
      Relay1.SetState(data.state); };

  // Connecting WiFi
  println("Initialize WiFi");
  if (data.factoryReset == true || data.wifiAP == true ) wifiAp();

  // Enable OTA update
  println("Starting OTA updates");
  ArduinoOTA.begin();

  if (data.factoryReset==false){
    if (data.factoryReset == true || data.ssid == "" ) wifiAp();
    else wifiConnect();

  //MQTT
  mqttStart();
  

  //Timers
  println("Starting timers");
  MessageTimer.setTime(data.status_delay*1000);
  MessageTimer.start();
  ServiceMessageTimer.setTime(data.avaible_delay*1000);
  ServiceMessageTimer.start();
  wifiApStaTimer.setTime(WIFIAPTIMER);
  wifiApStaTimer.attach(wifiApStaTimerHandler);

  println("Boot complete");
  println("-------------------------------");
  }
}

void portalStart(){
  println("Starting portal");
  portal.attachBuild(portalBuild);
  portal.disableAuth();
  portal.attach(portalAction);
  portal.OTA.attachUpdateBuild(OTAbuild);
  if (data.wifiAP == true ) portal.start(WIFI_AP);
  else portal.start(data.device_name);
  portal.enableOTA();
}

void wifiAp(){
  println("Create AP");

  String ssid = data.device_name;
  ssid += "AP";
  WiFi.mode(WIFI_AP);
  onSoftAPModeStationConnected = WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected& event)
  {
    portalStart();
  });

  onSoftAPModeStationDisconnected = WiFi.onSoftAPModeStationDisconnected([](const WiFiEventSoftAPModeStationDisconnected& event){
    data.wifiAP = false;
    memory.updateNow();
    WiFiApTimer.stop();
  });

  WiFi.softAP(ssid);
  IPAddress ip = WiFi.softAPIP();
  print("AP IP address: ");
  println(ip.toString());

  data.wifiConnectTry = 0;
  memory.updateNow();

  WiFiApTimer.setTime(WIFIAPTIMER);
  WiFiApTimer.setTimerMode();
  WiFiApTimer.attach(restart);
  WiFiApTimer.start();
}

void wifiConnect(){
  WiFi.mode(WIFI_AP_STA);
  WiFi.hostname(data.device_name);

 onStationModeConnected = WiFi.onStationModeConnected([](const WiFiEventStationModeConnected& event)
  {
    portalStart();
    wifiApStaTimer.start();
  });

  println("Wifi connecting...");
  WiFi.begin(data.ssid, data.password);
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    notConnectedCounter++;
    if (notConnectedCounter > 150)
    { // Reset board if not connected after 5s
      println("Resetting due to Wifi not connecting...");

      data.wifiConnectTry += 1;
      if(data.wifiConnectTry == 100)
        data.wifiAP = true;
      memory.updateNow();

      restart();
    }
  }

  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  print("Wifi connected, IP address: ");
  println(WiFi.localIP().toString());
}

void factoryReset(){
  println("Factory reset");
  memory.reset();
  data.factoryReset = true;
  memory.updateNow();
  restart();
}

void restart(){
  println("Rebooting...");
  println("-------------------------------");
  SendAvailableMessage("offline");
  mqttClient.loop();
  portal.tick();
  ESP.restart();
}

void ChangeRelayState(){
  println("Change relay state triggered");
  if(data.relaySaveStatus){
    println("Save relay state");
    data.state = Relay1.GetState();
    memory.updateNow();
  }
  publishRelay();
}
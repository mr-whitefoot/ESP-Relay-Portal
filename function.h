void println(String text){
  Serial.println(text);
  glog.println(text);
}

void print(String text){
  Serial.print(text);
  glog.print(text);
}

void startup(){
  Serial.begin(9600);
  //Журнал
  glog.start(1000);

  println("Booting...");
  println("Initialize EEPROM");
  EEPROM.begin(sizeof(data) + 1); // +3 на ключ
  memory.begin(0, 'k');         // запускаем менеджер памяти

  //Relay
  println("Initialize relay");
  Relay1.SetPin(0);
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
  println("Starting MQTT");
  client.setWifiCredentials(data.ssid, data.password);
  client.setMqttServer(data.mqttServerIp, data.mqttUsername, data.mqttPassword, data.mqttServerPort );
  client.setMqttClientName(data.device_name);
  //Настройка максимальной длинны сообщения MQTT
  client.setMaxPacketSize(1000);

  //Таймеры
  println("Starting timers");
  MessageTimer.setTime(data.status_delay*1000);
  MessageTimer.start();
  ServiceMessageTimer.setTime(data.avaible_delay*1000);
  ServiceMessageTimer.start();

  // подключаем конструктор портала и запускаем
  println("Starting portal");
  portal.attachBuild(portalBuild);
  portal.disableAuth();
  portal.attach(portalAction);
  portal.OTA.attachUpdateBuild(OTAbuild);
  portal.start(data.device_name);
  portal.enableOTA();

  println("Boot complete");
  }
}

void restart();
void wifiAp(){
  // создаём точку доступа
  println("Create AP");

  String ssid = data.device_name;
  ssid += "AP";
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  IPAddress ip = WiFi.softAPIP();
  print("AP IP address: ");
  println(ip.toString());

  data.wifiConnectTry = 0;
  memory.updateNow();

  portal.attachBuild(portalBuild);
  portal.attach(portalAction);
  portal.start(WIFI_AP);
  WiFiApTimer.setTime(WIFIAPTIMER);
  WiFiApTimer.setTimerMode();
  WiFiApTimer.attach(restart);
  WiFiApTimer.start();
  while (portal.tick()) {   // портал работает
    WiFiApTimer.tick(); };
}


void wifiConnect(){
  WiFi.hostname(data.device_name);
  WiFi.begin(data.ssid, data.password);
  uint32_t notConnectedCounter = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    println("Wifi connecting...");
    notConnectedCounter++;
    if (notConnectedCounter > 150)
    { // Reset board if not connected after 5s
      println("Resetting due to Wifi not connecting...");

      data.wifiConnectTry += 1;
      if(data.wifiConnectTry == 100);
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


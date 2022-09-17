void startup(){
  Serial.begin(9600);
  Serial.println();
  Serial.println("Booting...");
  Serial.println("Initialize EEPROM");
  EEPROM.begin(sizeof(data) + 1); // +3 на ключ
  memory.begin(0, 'k');         // запускаем менеджер памяти

  //Relay
  Relay1.SetPin(0);
  Relay1.SetInvertMode(data.relayInvertMode);
  
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
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid);
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);

  data.wifiConnectTry = 0;
  memory.updateNow();

  portal.attachBuild(portalBuild);
  portal.attach(portalAction);
  portal.start(WIFI_AP);
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

void factoryReset(){
  Serial.println("Factory reset"); 
  memory.reset();
  data.factoryReset = true;
  memory.updateNow();
  ESP.restart();
}
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
  // MQTT timers
  println("Starting MQTT timers");
  MessageTimer.setTime(data.status_delay*1000);
  MessageTimer.start();
  ServiceMessageTimer.setTime(data.avaible_delay*1000);
  ServiceMessageTimer.start();
}

int convertTimezoneToOffset(){
  if(data.time.timezone == 1 ) return -43200; //-12:00 
  if(data.time.timezone == 2 ) return -39600; //-11:00
  if(data.time.timezone == 3 ) return -36000; //-10:00
  if(data.time.timezone == 4 ) return -34200; //-09:30
  if(data.time.timezone == 5 ) return -32400; //-09:00
  if(data.time.timezone == 6 ) return -28800; //-08:00
  if(data.time.timezone == 7 ) return -25200; //-07:00
  if(data.time.timezone == 8 ) return -21600; //-06:00
  if(data.time.timezone == 9 ) return -18000; //-05:00
  if(data.time.timezone == 10 ) return -14400; //-04:00
  if(data.time.timezone == 11 ) return -12600; //-03:30
  if(data.time.timezone == 12 ) return -10800; //-03:00
  if(data.time.timezone == 13 ) return -7200; //-02:00
  if(data.time.timezone == 14 ) return -3600; //-01:00
  if(data.time.timezone == 15 ) return 0;     //UTC
  if(data.time.timezone == 16 ) return 3600;  //+01:00
  if(data.time.timezone == 17 ) return 7200;  //+02:00
  if(data.time.timezone == 18 ) return 10800; //+03:00
  if(data.time.timezone == 19 ) return 12600; //+03:30
  if(data.time.timezone == 19 ) return 14400; //+04:00
  if(data.time.timezone == 20 ) return 16200; //+04:30
  if(data.time.timezone == 21 ) return 18000; //+05:00
  if(data.time.timezone == 22 ) return 19800; //+05:30
  if(data.time.timezone == 23 ) return 20700; //+05:45
  if(data.time.timezone == 24 ) return 21600; //+06:00
  if(data.time.timezone == 25 ) return 23400; //+06:30
  if(data.time.timezone == 26 ) return 25200; //+07:00
  if(data.time.timezone == 27 ) return 28800; //+08:00
  if(data.time.timezone == 28 ) return 31500; //+08:45
  if(data.time.timezone == 29 ) return 32400; //+09:00
  if(data.time.timezone == 30 ) return 34200; //+09:30
  if(data.time.timezone == 31 ) return 36000; //+10:00
  if(data.time.timezone == 32 ) return 37800; //+10:30
  if(data.time.timezone == 33 ) return 39600; //+11:00
  if(data.time.timezone == 34 ) return 43200; //+12:00
  if(data.time.timezone == 35 ) return 46800; //+13:00
  if(data.time.timezone == 36 ) return 50400; //+14:00

  return 0;
}


void timerHandle(){
  int hours   = timeClient.getHours();
  int minutes = timeClient.getMinutes();
  int seconds = timeClient.getSeconds();

  for(int i=0; i<TIMER_COUNT; i++){
    if( data.time.timer[i].enable  == true &&
        data.time.timer[i].hours   == hours &&
        data.time.timer[i].minutes == minutes &&
        data.time.timer[i].seconds == seconds)
      {  
        println("Timer "+String(i)+" activating");
        if(data.time.timer[i].action == 0){Relay1.SetState(true);}
        if(data.time.timer[i].action == 1){Relay1.SetState(false);}
        if(data.time.timer[i].action == 2){Relay1.ResetState();}
      }
  }
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

  // WiFiAP timer
  println("Starting WiFiAP timer");
  wifiApStaTimer.setTime(WIFIAPTIMER);
  wifiApStaTimer.attach(wifiApStaTimerHandler);

  // Connecting WiFi
  println("Initialize WiFi");
  if (data.factoryReset == true || data.wifiAP == true ) {
    wifiAp();
  } else {
    wifiConnect();
  }

  // Enable OTA update
  println("Starting OTA updates");
  ArduinoOTA.begin();

  if (data.factoryReset==false){    
    //MQTT
    mqttStart();
  }

  //NTP 
  println("Starting NTP");
  timeClient.setPoolServerName("pool.ntp.org");
  timeClient.setTimeOffset(convertTimezoneToOffset());
  timeClient.begin();

  // Timers handler
  println("Starting timers handler");
  handleTimerDelay.setTime(1000);
  handleTimerDelay.attach(timerHandle);
  handleTimerDelay.start();

  println("Boot complete");
  println("-------------------------------");
}

void portalStart(){
  println("Starting portal");
  portal.attachBuild(portalBuild);
  portal.disableAuth();
  portal.attach(portalAction);
  portal.OTA.attachUpdateBuild(OTAbuild);
  portal.start(data.device_name);
  portal.enableOTA();
}
void wifiRestart(){
  data.wifiAP = false;
  memory.updateNow();
  restart();
}

void wifiAp(){
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

  WiFiApTimer.setTime(WIFIAPTIMER);
  WiFiApTimer.setTimerMode();
  WiFiApTimer.attach(wifiRestart);
  WiFiApTimer.start();

  onSoftAPModeStationConnected = WiFi.onSoftAPModeStationConnected([](const WiFiEventSoftAPModeStationConnected& event)
  {
    portalStart();
  });

  onSoftAPModeStationDisconnected = WiFi.onSoftAPModeStationDisconnected([](const WiFiEventSoftAPModeStationDisconnected& event){
    data.wifiAP = false;
    memory.updateNow();
    WiFiApTimer.stop();
  });
}

void wifiConnect(){
  WiFi.mode(WIFI_STA);
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
      if(data.wifiConnectTry >= 12){
        data.wifiAP = true;
        memory.updateNow();
        restart();
      }
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

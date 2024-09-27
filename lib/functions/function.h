void println(const String& text){
  Serial.println(text);
  glog.println(text);
}

void print(const String& text){
  Serial.print(text);
  glog.print(text);
}




int convertTimezoneToOffset(byte timezone){
  if(timezone == 1 ) return -43200; //-12:00 
  if(timezone == 2 ) return -39600; //-11:00
  if(timezone == 3 ) return -36000; //-10:00
  if(timezone == 4 ) return -34200; //-09:30
  if(timezone == 5 ) return -32400; //-09:00
  if(timezone == 6 ) return -28800; //-08:00
  if(timezone == 7 ) return -25200; //-07:00
  if(timezone == 8 ) return -21600; //-06:00
  if(timezone == 9 ) return -18000; //-05:00
  if(timezone == 10 ) return -14400; //-04:00
  if(timezone == 11 ) return -12600; //-03:30
  if(timezone == 12 ) return -10800; //-03:00
  if(timezone == 13 ) return -7200; //-02:00
  if(timezone == 14 ) return -3600; //-01:00
  if(timezone == 15 ) return 0;     //UTC
  if(timezone == 16 ) return 3600;  //+01:00
  if(timezone == 17 ) return 7200;  //+02:00
  if(timezone == 18 ) return 10800; //+03:00
  if(timezone == 19 ) return 12600; //+03:30
  if(timezone == 19 ) return 14400; //+04:00
  if(timezone == 20 ) return 16200; //+04:30
  if(timezone == 21 ) return 18000; //+05:00
  if(timezone == 22 ) return 19800; //+05:30
  if(timezone == 23 ) return 20700; //+05:45
  if(timezone == 24 ) return 21600; //+06:00
  if(timezone == 25 ) return 23400; //+06:30
  if(timezone == 26 ) return 25200; //+07:00
  if(timezone == 27 ) return 28800; //+08:00
  if(timezone == 28 ) return 31500; //+08:45
  if(timezone == 29 ) return 32400; //+09:00
  if(timezone == 30 ) return 34200; //+09:30
  if(timezone == 31 ) return 36000; //+10:00
  if(timezone == 32 ) return 37800; //+10:30
  if(timezone == 33 ) return 39600; //+11:00
  if(timezone == 34 ) return 43200; //+12:00
  if(timezone == 35 ) return 46800; //+13:00
  if(timezone == 36 ) return 50400; //+14:00

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

void portalStart(){
  println("Starting portal");
  portal.attachBuild(portalBuild);
  portal.disableAuth();
  portal.attach(portalAction);
  portal.OTA.attachUpdateBuild(OTAbuild);
  portal.start(db[keys::deviceName].toString().c_str());
  portal.enableOTA();
}



void dbSetup(){
  Serial.println("-----------------------------");
  Serial.println("Initialize database:");
  LittleFS.begin();

  if (!db.begin()){
    Serial.println("Database initialize error"); };

  db.init(keys::deviceName, "ESP Relay");
  db.init(keys::relayInvertMode, false);
  db.init(keys::saveRelayStatus, false);
  db.init(keys::relayState, false);
  db.init(keys::theme, LIGHT_THEME);
  db.init(keys::timezone, 14);

  db.init(mqtt::discoveryTopic, "homeassistant/switch/relay/config");
  db.init(mqtt::commandTopic, "homeassistant/switch/relay/set");
  db.init(mqtt::avaibleTopic, "homeassistant/switch/relay/avaible");
  db.init(mqtt::stateTopic, "homeassistant/switch/relay/state");
  db.init(mqtt::serverPort, 1883 );
  db.init(mqtt::status_delay, 10);
  db.init(mqtt::avaible_delay, 60);

  db.dump(Serial);
  println();
}


void startup(){
  Serial.begin(74880);
  //Log
  glog.start(1000);

  println();println();println();
  println("-------------------------------");
  println("Booting...");
  
  //Database
  dbSetup();
  

  //Relay
  println("Initialize relay");
  Relay1.SetPin(RELAY_PIN);
  Relay1.SetInvertMode(db[keys::relayInvertMode]);
  Relay1.ChangeStateCallback(ChangeRelayState);
  if(db[keys::saveRelayStatus]){ 
      println("Restore relay state");
      Relay1.SetState(db[keys::relayState]); };

  // WiFi
  wifiSetup(db[keys::deviceName], &db);
  

  // Enable OTA update
  println("Starting OTA updates");
  ArduinoOTA.begin();

  //MQTT
  mqttStart();

  //NTP 
  println("Starting NTP");
  timeClient.setPoolServerName("pool.ntp.org");
  timeClient.setTimeOffset(convertTimezoneToOffset(db[keys::timezone]));
  timeClient.begin();

  // Timers handler
  //data.time = db.get([keys::timer]);
  println("Starting timers handler");
  handleTimerDelay.setTime(1000);
  handleTimerDelay.attach(timerHandle);
  handleTimerDelay.start();


  portalStart();


  println("Boot complete");
  println("-------------------------------");
}

void factoryReset(){
  println("Factory reset");
  db.clear();
  db.update();
  restart();
}

void restart(){
  println("Rebooting...");
  println("-------------------------------");
  SendAvailableMessage("offline");
  mqttClient.loop();
  portal.tick();
  db.update();
  ESP.restart();
}

void ChangeRelayState(){
  println("Change relay state triggered");
  if(db[keys::saveRelayStatus]){
    println("Save relay state");
    db[keys::relayState] = Relay1.GetState();
    db.update();
  }
  publishRelay();
}

void createTimerUi(const int index){
  GP.BLOCK_TAB_BEGIN("Timer");
    GP.BOX_BEGIN(GP_EDGES);
      GP.LABEL("Timer"); GP.SWITCH("timerEnable"+String(index), data.time.timer[index].enable);
    GP.BOX_END();
    GP.SELECT("timerHours"+String(index),"00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23", data.time.timer[index].hours);
    GP.SELECT("timerMinutes"+String(index),"00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59",data.time.timer[index].minutes);
    GP.SELECT("timerSeconds"+String(index),"00,01,02,03,04,05,06,07,08,09,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59",data.time.timer[index].seconds);
    GP.BOX_BEGIN(GP_EDGES);
      GP.LABEL("Action"); GP.SELECT("timerAction"+String(index), "On,Off,Toggle", data.time.timer[index].action);
    GP.BOX_END();
  GP.BLOCK_END();
}

void copyTimer( const int index){
   portal.copyBool("timerEnable"+String(index),data.time.timer[index].enable);
   portal.copyInt("timerAction"+String(index),data.time.timer[index].action);
   portal.copyInt("timerHours"+String(index),data.time.timer[index].hours);
   portal.copyInt("timerMinutes"+String(index),data.time.timer[index].minutes);
   portal.copyInt("timerSeconds"+String(index),data.time.timer[index].seconds);
}



void portalBuild(){
  uint32_t timeleftAP = WiFiApTimer.timeLeft()/1000;

  GP.BUILD_BEGIN();
  if (data.theme == LIGHT_THEME ) GP.THEME(GP_LIGHT);
  else GP.THEME(GP_DARK);

  // Update components
  GP.UPDATE("signal,switch,mqttStatusLed,ipAddress,wifiAPTimer,time");


  // Configuration page
  if (portal.uri() == form.config) {
    GP.PAGE_TITLE("Configuration");
    GP.TITLE("Configuration");
    GP.HR();
    GP.BUTTON_LINK(form.preferences, "Preferences");
    GP.BUTTON_LINK(form.WiFiConfig, "WiFi configuration");
    GP.BUTTON_LINK(form.mqttConfig, "MQTT configuration");
    GP.BUTTON_LINK(form.factoryReset, "Factory reset");
    GP.BUTTON_LINK(form.firmwareUpgrade, "Firmware upgrade");
    GP.BUTTON("rebootButton", "Reboot");
    GP.HR();
    GP.BUTTON_LINK(form.root, "Back");

  //Log
  } else if (portal.uri() == form.log){
    GP.PAGE_TITLE("Log");
    GP.AREA_LOG(glog, 20);
    GP.BUTTON_LINK(form.root, "Back");

  //Timers
  } else if (portal.uri() == form.timers){
    GP.FORM_BEGIN(form.timers);
      GP.PAGE_TITLE("Timers");
      GP.TITLE("Timers");
      GP.HR();
      for(int i=0; i<TIMER_COUNT; i++){
        createTimerUi(i);
      }
      GP.HR();
      GP.SUBMIT("Save");
    GP.FORM_END();

     GP.BUTTON_LINK(form.root, "Back");

  //Preferences
  } else if (portal.uri() == form.preferences){
    GP.FORM_BEGIN(form.preferences);
      GP.PAGE_TITLE("Preferences");
      GP.TITLE("Preferences");
      GP.HR();
      GP.BLOCK_TAB_BEGIN("Device name");
        GP.TEXT("label", "Label", data.label); GP.BREAK();
        GP.TEXT("device_name", "Device name", data.device_name); GP.BREAK();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("Settings");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Theme");   GP.SELECT("theme", "Light,Dark", data.theme);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Relay invert mode"); GP.SWITCH("relayInvertMode", data.relayInvertMode);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Save relay status"); GP.SWITCH("relaySaveStatus", data.relaySaveStatus);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Timezone"); 
          GP.SELECT("timezone", "-12:00,-11:00,-10:00,-09:30,-09:00,-08:00,-07:00,-06:00,-05:00,-04:00,-03:30,-03:00,-02:00,-01:00,00:00,+01:00,+02:00,+03:00,+03:30,+04:00,+04:30,+05:00,+05:30,+05:45,+06:00,+06:30,+07:00,+08:00,+08:45,+09:00,+09:30,+10:00,+10:30,+11:00,+12:00,+13:00,+14:00", data.time.timezone);
        GP.BOX_END();
      GP.BLOCK_END();


      GP.BLOCK_TAB_BEGIN("Information");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Firmware version");
          GP.LABEL(sw_version);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Release date");
          GP.LABEL(release_date);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("MAC");
          GP.LABEL(WiFi.macAddress());
        GP.BOX_END();
      GP.BLOCK_END();

      GP.HR();
      GP.SUBMIT("Save");
    GP.FORM_END();
    GP.BUTTON_LINK(form.config, "Back");


    // WiFi configuration page
  } else if (portal.uri() == form.WiFiConfig) {
      GP.FORM_BEGIN(form.WiFiConfig);
        GP.PAGE_TITLE("WiFi configuration");
        GP.TITLE("WiFi");
        GP.HR();

        GP.BLOCK_TAB_BEGIN("Information");
          if (WiFi.status() == WL_CONNECTED){
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("WiFi status"); GP.LED_GREEN("WiFiLed", true);
            GP.BOX_END();
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("Signal"); GP.LABEL("","signal");
            GP.BOX_END();
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("IP address"); GP.LABEL(WiFi.localIP().toString(),"ipAddress");
            GP.BOX_END();}
          else {
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("WiFi status"); GP.LED_GREEN("WiFiLed", false);
            GP.BOX_END();
          }
        GP.BLOCK_END();

        GP.BLOCK_TAB_BEGIN("Settings");
          GP.TEXT("ssid", "SSID", data.ssid);GP.BREAK();
          GP.PASS("pass", "Password", data.password);GP.BREAK();
        GP.BLOCK_END();

        GP.HR();
        GP.SUBMIT("Save");
        GP.BUTTON_LINK(form.config, "Back");
      GP.FORM_END();

    // MQTT configuration page
  } else if (portal.uri() == form.mqttConfig) {
    GP.FORM_BEGIN(form.mqttConfig);
      GP.PAGE_TITLE("MQTT configuration");
      GP.TITLE("MQTT");
      GP.HR();

      GP.BLOCK_TAB_BEGIN("Information");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Status"); GP.LED_GREEN("mqttStatusLed", mqttClient.isConnected());
        GP.BOX_END();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("Server");
        GP.TEXT("mqttServerIp", "Server", data.mqttServerIp); GP.BREAK();
        GP.NUMBER("mqttServerPort", "Port", data.mqttServerPort); GP.BREAK();
        GP.TEXT("mqttUsername", "Username", data.mqttUsername); GP.BREAK();
        GP.PASS("mqttPassword", "Password", data.mqttPassword); GP.BREAK();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("MQTT Message periods");
        GP.NUMBER("avaible_delay", "Avaible", data.avaible_delay); GP.BREAK();
        GP.NUMBER("status_delay", "Message", data.status_delay); GP.BREAK();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("MQTT topics");
        GP.LABEL("Discovery topic"); GP.BREAK();
        GP.TEXT("discoveryTopic", "Discovery topic", data.discoveryTopic); GP.BREAK();
        GP.LABEL("Command topic"); GP.BREAK();
        GP.TEXT("commandTopic", "Command topic", data.commandTopic); GP.BREAK();
        GP.LABEL("Avaible topic"); GP.BREAK();
        GP.TEXT("avaibleTopic", "Avaible topic", data.avaibleTopic); GP.BREAK();
        GP.LABEL("State topic"); GP.BREAK();
        GP.TEXT("stateTopic", "State topic", data.stateTopic); GP.BREAK();
      GP.BLOCK_END();

      GP.HR();
      GP.SUBMIT("Save and reboot");
      GP.BUTTON_LINK(form.config, "Back");;
    GP.FORM_END();

    //Factory reset page
  } else if (portal.uri() == form.factoryReset) {
    GP.FORM_BEGIN(form.factoryReset);
      GP.PAGE_TITLE("Factory reset");
      GP.TITLE("Factory reset");
      GP.HR();
      GP.BOX_BEGIN(GP_EDGES);
        GP.LABEL("I'm really understand what I do");
        GP.CHECK("resetAllow", resetAllow);  GP.BREAK();
      GP.BOX_END();

      GP.HR();
      GP.SUBMIT("Factory reset");
      GP.BUTTON_LINK(form.config, "Back");;
    GP.FORM_END();

    // Root page, "/"
  } else {
    GP.PAGE_TITLE("Portal");
    GP.FORM_BEGIN(form.root);
       GP.BLOCK_TAB_BEGIN("Control");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL( data.label ); GP.SWITCH("switch", Relay1.GetState());
        GP.BOX_END();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("WiFi");
        if (WiFi.status() == WL_CONNECTED){
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Status");GP.LED_GREEN("WiFiLed", true);
          GP.BOX_END();
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Signal"); GP.LABEL("","signal");
          GP.BOX_END();
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("IP address"); GP.LABEL(WiFi.localIP().toString(),"ipAddress");
          GP.BOX_END();
        }else{
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Status");GP.LED_GREEN("WiFiLed", false);
          GP.BOX_END();
        }
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("MQTT");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Status"); GP.LED_GREEN("mqttStatusLed", mqttClient.isConnected());
        GP.BOX_END();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("Information");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Time"); 
          GP.LABEL("","time");
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Firmware version");
          GP.LABEL(sw_version);
        GP.BOX_END();
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Release date");
          GP.LABEL(release_date);
        GP.BOX_END();
        if (WiFiApTimer.active()){
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Restart in");
            GP.LABEL(String(timeleftAP),"wifiAPTimer");
          GP.BOX_END();
        };
      GP.BLOCK_END();
      GP.HR();
      GP.BUTTON_LINK(form.timers, "Timers");
      GP.BUTTON_LINK(form.config, "Configuration");
      GP.BUTTON_LINK(form.log, "Log");
    GP.FORM_END();
  }
  GP.BUILD_END();
}

void portalCheckForm(){
  if (portal.form()) {
    //WiFi config
    if (portal.form(form.WiFiConfig)) {
      portal.copyStr("ssid", data.ssid);
      portal.copyStr("pass", data.password);
      data.factoryReset = false;
      data.wifiAP = false;
      memory.updateNow();
      wifiConnect();

    // Factory reset
    } else if(portal.form(form.factoryReset)){
      Serial.println("Factory reset");
      if(portal.getCheck("resetAllow"))
        factoryReset();

    // Preferences
    } else if(portal.form(form.preferences)){
      portal.copyStr("label", data.label);
      portal.copyStr("device_name", data.device_name);
      data.relayInvertMode = portal.getCheck("relayInvertMode");
      Relay1.SetInvertMode( data.relayInvertMode );
      portal.copyInt("theme", data.theme);
      portal.copyInt("timezone", data.time.timezone);
      timeClient.setTimeOffset(convertTimezoneToOffset());
      memory.updateNow();

      //MQTT Config
    } else if(portal.form(form.mqttConfig)){
      portal.copyStr("mqttServerIp", data.mqttServerIp);
      data.mqttServerPort = portal.getInt("mqttServerPort");
      portal.copyStr("mqttUsername", data.mqttUsername);
      portal.copyStr("mqttPassword", data.mqttPassword);
      data.avaible_delay = portal.getInt("avaible_delay");
      data.status_delay = portal.getInt("status_delay");
      portal.copyStr("discoveryTopic", data.discoveryTopic);
      portal.copyStr("commandTopic", data.commandTopic);
      portal.copyStr("avaibleTopic", data.avaibleTopic);
      portal.copyStr("stateTopic", data.stateTopic);
      memory.updateNow();
      restart();

      //Timers
    } else if(portal.form(form.timers)){
        for(int i=0; i < TIMER_COUNT; i++){ copyTimer(i); };
        memory.updateNow();
    }
  }

  if (portal.update()){
    long rssi = WiFi.RSSI();
    int strength = map(rssi, -80, -20, 0, 100);
    String wifiStrength = String(strength)+"%";
    portal.updateString("signal", wifiStrength);

    portal.updateInt("switch", Relay1.GetState());
    portal.updateInt("mqttStatusLed",mqttClient.isConnected());
    String ipAdress = WiFi.localIP().toString();
    portal.updateString("ipAddress", ipAdress);

    uint32_t timeleftAP = WiFiApTimer.timeLeft()/1000;
    portal.updateInt("wifiAPTimer", timeleftAP);

    String time = timeClient.getFormattedTime();
    portal.updateString("time", time);

    portal.updateLog(glog);
  }
}

void portalAction(){
  portalCheckForm();

  if (portal.click()){
    Serial.println("Portal click");

    if (portal.click("switch")){ Relay1.SetState( portal.getCheck("switch") ); }
    if (portal.click("relayInverMode")){
      data.relayInvertMode = portal.getCheck("relayInverMode");
      Relay1.SetInvertMode( data.relayInvertMode );
      memory.updateNow();
    }
    if (portal.click("relaySaveStatus")){
      data.relaySaveStatus = portal.getCheck("relaySaveStatus");
      memory.updateNow();
    }
    if (portal.click("rebootButton")){ restart(); }
  }
}

//Custom OTA page
void OTAbuild(bool UpdateEnd, const String& UpdateError) {
  GP.BUILD_BEGIN(400);
    #ifndef GP_OTA_LIGHT
      GP.THEME(GP_DARK);
    #else
      GP.THEME(GP_LIGHT);
    #endif
    GP.PAGE_TITLE(F("Firmware upgrade"));
    if (!UpdateEnd) {
      GP.BLOCK_TAB_BEGIN(F("Firmware upgrade"));
        GP.OTA_FIRMWARE(F("OTA firmware"), GP_GREEN, true);
      GP.BLOCK_END();
      GP.BUTTON_LINK(form.config, "Back");
    } else if (UpdateError.length()) {
      GP.BLOCK_TAB_BEGIN(F("Firmware upgrade"));
        GP.TITLE(String(F("Update error: ")) + UpdateError);
        GP.BUTTON_LINK(form.firmwareUpgrade, F("Refresh"));
      GP.BLOCK_END();

    } else {
      GP.BLOCK_TAB_BEGIN(F("Firmware upgrade"));
        GP.TITLE(F("Update Success!"));
        GP.TITLE(F("Rebooting..."));
      GP.BLOCK_END();
      GP.BUTTON_LINK(form.root, "Home");
    }
  GP.BUILD_END();
}

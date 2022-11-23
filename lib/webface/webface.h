void portalBuild(){
  uint32_t timeleftAP = WiFiApTimer.timeLeft()/1000;
  
  GP.BUILD_BEGIN();
  if (data.theme == LIGHT_THEME ) GP.THEME(GP_LIGHT);
  else GP.THEME(GP_DARK);

  // Update components
  GP.UPDATE("signal,switch,mqttStatusLed,ipAddress,wifiAPTimer");


  // Configuration page
  if (portal.uri() == form.config) {
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
    GP.AREA_LOG(glog, 20);
    GP.BUTTON_LINK(form.root, "Back");

  //Preferences
  } else if (portal.uri() == form.preferences){
    GP.FORM_BEGIN(form.preferences);
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
      GP.BLOCK_END();
      
      GP.BLOCK_TAB_BEGIN("Information");
        GP.LABEL("Version: "+version); GP.BREAK();
        GP.LABEL("MAC adress: "+WiFi.macAddress()); GP.BREAK();
      GP.BLOCK_END();

      GP.HR();
      GP.SUBMIT("Save");
    GP.FORM_END();
    GP.BUTTON_LINK(form.config, "Back");


    // WiFi configuration page 
  } else if (portal.uri() == form.WiFiConfig) {
      GP.FORM_BEGIN(form.WiFiConfig);
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
      GP.TITLE( "Factory reset" );
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
          GP.LABEL("Firmware version");
          GP.LABEL(version);
        GP.BOX_END();
        if (WiFiApTimer.active()){
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Restart in");
            GP.LABEL(String(timeleftAP),"wifiAPTimer");
          GP.BOX_END();
        };
      GP.BLOCK_END();
      GP.HR();
      GP.BUTTON_LINK(form.config, "Configuration");
      GP.BUTTON_LINK(form.log, "Log");
    GP.FORM_END();
  }
  BUILD_END();
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

    portal.updateLog(glog);
  }
}

void portalAction(){
  portalCheckForm();
    
  if (portal.click()){
    Serial.println("Portal click");
   
    if (portal.click("switch")){
      Relay1.SetState( portal.getCheck("switch") );
    }
    if (portal.click("relayInverMode")){
      data.relayInvertMode = portal.getCheck("relayInverMode");
      Relay1.SetInvertMode( data.relayInvertMode );
      memory.updateNow();
    }
    if (portal.click("relaySaveStatus")){
      data.relaySaveStatus = portal.getCheck("relaySaveStatus");
      memory.updateNow();
    }
    if (portal.click("rebootButton")){
      restart();
    }
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

    if (!UpdateEnd) {
      GP.BLOCK_TAB_BEGIN(F("Firmware upgrade"));
        GP.OTA_FIRMWARE();
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
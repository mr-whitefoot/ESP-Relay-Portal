
void portalBuild(){
  long rssi = WiFi.RSSI();
  int strength = map(rssi, -80, -20, 0, 100);
  uint32_t timeleftAP = WiFiApTimer.timeLeft()/1000;
  
  GP.BUILD_BEGIN();
  GP.THEME(GP_DARK);

  // Страница конфигурации
  if (portal.uri() == form.config.c_str()) {
    GP.BUTTON_LINK(form.preferences.c_str(), "Preferences"); GP.BREAK();
    GP.BUTTON_LINK(form.WiFiConfig.c_str(), "WiFi configuration"); GP.BREAK();
    GP.BUTTON_LINK(form.mqttConfig.c_str(), "MQTT configuration"); GP.BREAK();
    GP.BUTTON_LINK(form.factoryReset.c_str(), "Factory reset"); GP.BREAK();
    GP.BUTTON_LINK(form.firmwareUpgrade.c_str(), "Firmware upgrade"); GP.BREAK();
    GP.BUTTON_LINK("/", "Back");

  } else if (portal.uri() == form.preferences){
    GP.FORM_BEGIN(form.preferences.c_str());
      GP.TITLE("Preferences");
      GP.HR();
      GP.BLOCK_TAB_BEGIN("Device name");
        GP.TEXT("label", "Label", data.label); GP.BREAK();
        GP.TEXT("device_name", "Device name", data.device_name); GP.BREAK();
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("Settings");
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
      
      GP.SUBMIT("Save");    GP.BREAK();
    GP.FORM_END();    GP.FORM_END();
    GP.BUTTON_LINK(form.config.c_str(), "Back");


    // страница конфигурации wifi 
  } else if (portal.uri() == form.WiFiConfig) {
      GP.FORM_BEGIN(form.WiFiConfig.c_str());
        GP.TITLE("WiFi");
        GP.HR();

        GP.BLOCK_TAB_BEGIN("Information");
          if (WiFi.status() == WL_CONNECTED){
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("WiFi status"); GP.LED_GREEN("WiFiLed", true);
            GP.BOX_END();
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("Signal"); GP.LABEL(String(strength)+"%");
            GP.BOX_END();
            GP.BOX_BEGIN(GP_EDGES);
              GP.LABEL("IP address"); GP.LABEL(WiFi.localIP().toString());
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
        
        GP.SUBMIT("Save and reboot");    GP.BREAK();
        GP.BUTTON_LINK(form.config.c_str(), "Back");
      GP.FORM_END();

    // страница конфигурации MQTT
  } else if (portal.uri() == form.mqttConfig) {
    GP.FORM_BEGIN(form.mqttConfig.c_str());
      GP.TITLE("MQTT");
      GP.HR();

      GP.BLOCK_TAB_BEGIN("Information");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Status"); 
          if (client.isConnected()){ GP.LED_GREEN("MQTT", true);}
          else{ GP.LED_GREEN("MQTT", false);}
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

      GP.SUBMIT("Save and reboot"); GP.BREAK();
      GP.BUTTON_LINK(form.mqttTopic.c_str(), "MQTT Topic"); GP.BREAK();
      GP.BUTTON_LINK(form.config.c_str(), "Back"); GP.BREAK();
    GP.FORM_END();
    
  // Страница конфигурации топиков mqtt
  } else if (portal.uri() == form.mqttTopic) {
    GP.FORM_BEGIN(form.mqttTopic.c_str());
      GP.TITLE("MQTT topics");
      GP.HR();
      GP.BLOCK_BEGIN();
        GP.LABEL("Discovery topic"); GP.BREAK();
        GP.TEXT("discoveryTopic", "Discovery topic", data.discoveryTopic); GP.BREAK();
        GP.LABEL("Command topic"); GP.BREAK();
        GP.TEXT("commandTopic", "Command topic", data.commandTopic); GP.BREAK();
        GP.LABEL("Avaible topic"); GP.BREAK();
        GP.TEXT("avaibleTopic", "Avaible topic", data.avaibleTopic); GP.BREAK();
        GP.LABEL("State topic"); GP.BREAK();
        GP.TEXT("stateTopic", "State topic", data.stateTopic); GP.BREAK();
      GP.BLOCK_END();

      GP.SUBMIT("Save and reboot"); GP.BREAK();
      GP.BUTTON_LINK(form.mqttConfig.c_str(), "Back");
    GP.FORM_END();   

    //Сброс до заводских настроек
  } else if (portal.uri() == form.factoryReset) {
    GP.FORM_BEGIN(form.factoryReset.c_str());
      GP.TITLE( "Factory reset" );
      GP.HR();
      GP.BOX_BEGIN(GP_EDGES);
        GP.LABEL("I'm really understand what I do");           
        GP.CHECK("resetAllow", resetAllow);  GP.BREAK(); 
      GP.BOX_END();
      GP.SUBMIT("Factory reset"); GP.BREAK();
      GP.BUTTON_LINK(form.config.c_str(), "Back");  GP.BREAK();
    GP.FORM_END();
    
    // главная страница, корень, "/"
  } else {
    GP.FORM_BEGIN(form.status.c_str());
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
            GP.LABEL("Signal"); GP.LABEL(String(strength)+"%");
          GP.BOX_END();
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("IP address"); GP.LABEL(WiFi.localIP().toString());
          GP.BOX_END(); 
        }else{
          GP.BOX_BEGIN(GP_EDGES);
            GP.LABEL("Status");GP.LED_GREEN("WiFiLed", false);
          GP.BOX_END(); 
        }
      GP.BLOCK_END();
      
      GP.BLOCK_TAB_BEGIN("MQTT");
        GP.BOX_BEGIN(GP_EDGES);
          GP.LABEL("Status");
          if (client.isConnected()){ GP.LED_GREEN("MQTT", true); }
          else{ GP.LED_GREEN("MQTT", false); }
        GP.BOX_END(); 
      GP.BLOCK_END();

      GP.BLOCK_TAB_BEGIN("Information");
        GP.LABEL("Relay version: "+version); GP.BREAK();
        if (WiFiApTimer.active()){
          GP.LABEL("Restart in:");
          GP.LABEL(String(timeleftAP)); GP.BREAK();
        };
      GP.BLOCK_END();
      
      GP.BUTTON_LINK(form.config.c_str(), "Configuration"); GP.BREAK();
    GP.FORM_END();
  }
  BUILD_END();
}


void portalAction(){
  Serial.println("Portal Action");
  portalCheck();
    
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
    
  }
}

void portalCheck(){

  Serial.print("Portal form");
  Serial.println(portal.form());
  if (portal.form()) {
    Serial.println("Portal form handle");
    
    //WiFi config
    if (portal.form(form.WiFiConfig)) {
      portal.copyStr("ssid", data.ssid);
      portal.copyStr("pass", data.password);
      data.factoryReset = false;
      data.wifiAP = false;
      memory.updateNow();
      ESP.restart();
    
    // Factory reset
    } else if(portal.form(form.factoryReset)){
      Serial.println("Factory reset"); 
      if(portal.getCheck("resetAllow"));
        factoryReset();
    
    // Preferences
    } else if(portal.form(form.preferences)){
      portal.copyStr("label", data.label);
      portal.copyStr("device_name", data.device_name);
      data.relayInvertMode = portal.getCheck("relayInvertMode");
      Relay1.SetInvertMode( data.relayInvertMode );
      memory.updateNow();
      
      //MQTT Config
    } else if(portal.form(form.mqttConfig)){
      portal.copyStr("mqttServerIp", data.mqttServerIp);
      data.mqttServerPort = portal.getInt("mqttServerPort");
      portal.copyStr("mqttUsername", data.mqttUsername);
      portal.copyStr("mqttPassword", data.mqttPassword);
      data.avaible_delay = portal.getInt("avaible_delay");
      data.status_delay = portal.getInt("status_delay");
      memory.updateNow();
      ESP.restart();

    //Топики
    } else if(portal.form(form.mqttTopic)){
      portal.copyStr("discoveryTopic", data.discoveryTopic);
      portal.copyStr("commandTopic", data.commandTopic);
      portal.copyStr("avaibleTopic", data.avaibleTopic);
      portal.copyStr("stateTopic", data.stateTopic);
      memory.updateNow();
      ESP.restart();
    }
  }
}
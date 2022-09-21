
void portalBuild(){
  String p;
  long rssi = WiFi.RSSI();
  int strength = map(rssi, -80, -20, 0, 100);
  
  BUILD_BEGIN(p);
  add.THEME(GP_DARK);

// Страница конфигурации
if (portal.uri() == form.config.c_str()) {
    add.BUTTON_LINK(form.preferences.c_str(), "Preferences"); add.BREAK();
    add.BUTTON_LINK(form.WiFiConfig.c_str(), "WiFi configuration"); add.BREAK();
    add.BUTTON_LINK(form.mqttConfig.c_str(), "MQTT configuration"); add.BREAK();
    add.BUTTON_LINK(form.factoryReset.c_str(), "Factory reset"); add.BREAK();
    add.BUTTON_LINK(form.firmwareUpgrade.c_str(), "Firmware upgrade"); add.BREAK();
    add.BUTTON_LINK("/", "Back");

  } else if (portal.uri() == form.preferences){
    add.FORM_BEGIN(form.preferences.c_str());
    add.LABEL("Preferences");
    add.BLOCK_BEGIN();
    add.TEXT("label", "Label", data.label); add.BREAK();
    add.TEXT("device_name", "Device name", data.device_name); add.BREAK();
    add.LABEL( "Relay invert mode" ); add.SWITCH("relayInvertMode", data.relayInvertMode); add.BREAK();
    add.BLOCK_END();
    
    add.BLOCK_BEGIN();
    add.LABEL_MINI("Version: "+version); add.BREAK();
    add.LABEL_MINI("MAC adress: "+WiFi.macAddress()); add.BREAK();
    add.BLOCK_END();
    
    add.SUBMIT("Save");    add.BREAK();
    add.FORM_END();    add.FORM_END();
    add.BUTTON_LINK(form.config.c_str(), "Back");


    // страница конфигурации wifi 
  } else if (portal.uri() == form.WiFiConfig) {
    add.FORM_BEGIN(form.WiFiConfig.c_str());
    add.LABEL("WIFI");

    add.BLOCK_BEGIN();
    add.LABEL_MINI("WiFi status"); 
    if (WiFi.status() == WL_CONNECTED){
      add.LED_GREEN("WiFiLed", true); add.BREAK();
      add.LABEL_MINI("Signal:"); add.LABEL_MINI(strength); add.LABEL_MINI("%"); add.BREAK();
      add.LABEL_MINI("IP address: "+WiFi.localIP().toString()); add.BREAK();}
    else { add.LED_GREEN("WiFiLed", false); add.BREAK(); }
    add.BLOCK_END();

    add.BLOCK_BEGIN();
    add.TEXT("ssid", "SSID", data.ssid);
    add.BREAK();
    add.PASS("pass", "Password", data.password);
    add.BLOCK_END();
    

    add.SUBMIT("Save and reboot");    add.BREAK();
    add.BUTTON_LINK(form.config.c_str(), "Back");
    add.FORM_END();


    // страница конфигурации MQTT
  } else if (portal.uri() == form.mqttConfig) {
    add.FORM_BEGIN(form.mqttConfig.c_str());
    add.LABEL("MQTT Server");

    add.BLOCK_BEGIN();
    add.LABEL_MINI("MQTT status"); 
    if (client.isConnected()){ add.LED_GREEN("MQTT", true); add.BREAK();}
    else{ add.LED_GREEN("MQTT", false); add.BREAK();}
    add.BLOCK_END();

    add.BLOCK_BEGIN();
    add.TEXT("mqttServerIp", "Server", data.mqttServerIp); add.BREAK();
    add.NUMBER("mqttServerPort", "Port", data.mqttServerPort); add.BREAK();
    add.TEXT("mqttUsername", "Username", data.mqttUsername); add.BREAK();
    add.PASS("mqttPassword", "Password", data.mqttPassword); add.BREAK();
    add.BLOCK_END();

    add.LABEL("MQTT Message periods");
    add.BLOCK_BEGIN();
    add.NUMBER("avaible_delay", "Avaible", data.avaible_delay); add.BREAK();
    add.NUMBER("status_delay", "Message", data.status_delay); add.BREAK();
    add.BLOCK_END();

    add.SUBMIT("Save and reboot"); add.BREAK();
    add.BUTTON_LINK(form.mqttTopic.c_str(), "MQTT Topic"); add.BREAK();
    add.BUTTON_LINK(form.config.c_str(), "Back"); add.BREAK();
    add.FORM_END();
    
  // Страница конфигурации топиков mqtt
  } else if (portal.uri() == form.mqttTopic) {
    add.FORM_BEGIN(form.mqttTopic.c_str());
    add.TITLE("MQTT topics");
    add.BLOCK_BEGIN();
    add.LABEL("Discovery topic"); add.BREAK();
    add.TEXT("discoveryTopic", "Discovery topic", data.discoveryTopic); add.BREAK();
    add.LABEL("Command topic"); add.BREAK();
    add.TEXT("commandTopic", "Command topic", data.commandTopic); add.BREAK();
    add.LABEL("Avaible topic"); add.BREAK();
    add.TEXT("avaibleTopic", "Avaible topic", data.avaibleTopic); add.BREAK();
    add.LABEL("State topic"); add.BREAK();
    add.TEXT("stateTopic", "State topic", data.stateTopic); add.BREAK();
    add.BLOCK_END();

    add.SUBMIT("Save and reboot"); add.BREAK();
    add.BUTTON_LINK(form.mqttConfig.c_str(), "Back");
    add.FORM_END();   

    //Сброс до заводских настроек
  } else if (portal.uri() == form.factoryReset) {
    add.FORM_BEGIN(form.factoryReset.c_str());
    add.TITLE( "Factory reset" );
    add.LABEL_MINI("I'm really understand what I do"); 
    add.CHECK("resetAllow", resetAllow);  add.BREAK(); add.BREAK();
    add.SUBMIT("Factory reset"); add.BREAK();
    add.BUTTON_LINK(form.config.c_str(), "Back");  add.BREAK();
    add.FORM_END();
    
    // главная страница, корень, "/"
  } else {
    add.FORM_BEGIN(form.status.c_str());
    add.BLOCK_BEGIN();
    add.LABEL( data.label ); add.SWITCH("switch", Relay1.GetState()); add.BREAK();
    add.BLOCK_END();
    add.BLOCK_BEGIN();
    
    add.LABEL_MINI("WiFi status"); 
    if (WiFi.status() == WL_CONNECTED){
      add.LED_GREEN("WiFiLed", true); add.BREAK();
      add.LABEL_MINI("Signal:"); add.LABEL_MINI(strength); add.LABEL_MINI("%"); add.BREAK();
      add.LABEL_MINI("IP address: "+WiFi.localIP().toString()); add.BREAK();}
    else add.LED_GREEN("WiFiLed", false);
    add.LABEL_MINI("MAC adress: "+WiFi.macAddress()); add.BREAK();
    add.BLOCK_END();
    
    add.BLOCK_BEGIN();
    add.LABEL_MINI("MQTT status"); 
    if (client.isConnected()){ add.LED_GREEN("MQTT", true); add.BREAK();}
    else{ add.LED_GREEN("MQTT", false); add.BREAK();}
    add.BLOCK_END();

    add.BLOCK_BEGIN();
    add.LABEL_MINI("Version: "+version); add.BREAK();
    add.BLOCK_END();
    
    add.BUTTON_LINK(form.config.c_str(), "Configuration"); add.BREAK();
    add.FORM_END();
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
      publishRelay();
    }
    if (portal.click("relayInverMode")){
      data.relayInvertMode = portal.getCheck("relayInverMode");
      Relay1.SetInvertMode( data.relayInvertMode );
      publishRelay();
      memory.updateNow();
    }
  }
}

void portalCheck(){

  Serial.print("Portal form");
  Serial.println(portal.form());
  if (portal.form()) {
    Serial.println("Portal form handle");
    if (portal.form(form.WiFiConfig)) {
      portal.copyStr("ssid", data.ssid);
      portal.copyStr("pass", data.password);
      data.factoryReset = false;
      data.wifiAP = false;
      memory.updateNow();
      ESP.restart();
    } else if(portal.form(form.factoryReset)){
      Serial.println("Factory reset"); 
      if(portal.getCheck("resetAllow"));
        factoryReset();
    } else if(portal.form(form.preferences)){
      portal.copyStr("label", data.label);
      portal.copyStr("device_name", data.device_name);
      data.relayInvertMode = portal.getCheck("relayInvertMode");
      Relay1.SetInvertMode( data.relayInvertMode );
      publishRelay();
      memory.updateNow();
      
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
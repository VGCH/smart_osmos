String JSON_DATA(){
          char jsonBuffer[2048] = {0};
          
          DynamicJsonDocument chan1(2048);
                              chan1["key"]          = "real_time";
                              chan1["r_cur_in"]     = String(settings.totalIN);
                              chan1["r_cur_out"]    = String(settings.totalOUT);
                              chan1["wi-fi_s"]      = String(WiFi.SSID());
                              chan1["wi-fi_st"]     = Wi_Fi_status();
                              chan1["wi-fi_rssi"]   = WiFi.RSSI();
                              chan1["tds"]          = String(TDS_Data_s);
                              chan1["prs"]          = String(prs_sensor_data_s);
                              chan1["mqttstat"]     = MQTT_status();
                              chan1["lp5mIn"]       = String(lp5mIn);
                              chan1["lp5mOut"]      = String(lp5mOut);
                              chan1["effic"]        = String(effect());
                              chan1["timework"]     = millis2time();
                              chan1["res_P"]        = res_prs(settings.prefiltr,  settings.totalIN_P);
                              chan1["res_M"]        = res_prs(settings.membrana,  settings.totalIN_M);
                              chan1["res_PO"]       = res_prs(settings.postfiltr, settings.totalIN_PO);
                              chan1["ip"]           = ip_address();
                              chan1["flow_in"]      = String(flowIn);
                              chan1["flow_out"]     = String(flowOut);
                              chan1["ota_en"]       = ota_update;
                              chan1["ota_link"]     = get_chip_id()+".local";
           serializeJson(chan1, jsonBuffer, sizeof(jsonBuffer));
           
  return jsonBuffer;
}

String JSON_DATA_MQTT(){
          char jsonBuffer[1024] = {0};
          
          DynamicJsonDocument chan1(1024);              
                              chan1["r_cur_in"]     = String(settings.totalIN);
                              chan1["r_cur_out"]    = String(settings.totalOUT);
                              chan1["tds"]          = String(TDS_Data_s);
                              chan1["prs"]          = String(prs_sensor_data_s);
                              chan1["lp5mIn"]       = String(lp5mIn);
                              chan1["lp5mOut"]      = String(lp5mOut);
                              chan1["effic"]        = String(effect());
                              chan1["res_P"]        = String(res_prs(settings.prefiltr,  settings.totalIN_P));
                              chan1["res_M"]        = String(res_prs(settings.membrana,  settings.totalIN_M));
                              chan1["res_PO"]       = String(res_prs(settings.postfiltr, settings.totalIN_PO));
                              chan1["flow_in"]      = String(flowIn);
                              chan1["flow_out"]     = String(flowOut);
           serializeJson(chan1, jsonBuffer, sizeof(jsonBuffer));
           
  return jsonBuffer;
}

String JSON_config_DATA(){
          char jsonBuffer[1024] = {0};
          
          DynamicJsonDocument chan1(1024);
                              chan1["key"]           = "config_data";
                              chan1["co_c_in"]       = settings.counter_coeff_in;
                              chan1["co_c_out"]      = settings.counter_coeff_out;
                              chan1["prefiltr"]      = settings.prefiltr;
                              chan1["membrana"]      = settings.membrana;
                              chan1["postfiltr"]     = settings.postfiltr;
                              chan1["ssid"]          = String(settings.mySSID);
                              chan1["mqtt_serv"]     = String(settings.mqtt_serv);
                              chan1["mqtt_user"]     = String(settings.mqtt_user);
                              chan1["mqtt_topic"]    = String(settings.mqtt_topic);
                              
           serializeJson(chan1, jsonBuffer, sizeof(jsonBuffer));
           
  return jsonBuffer;
}
String JSON_scan_end(){
          char jsonBuffer[1024] = {0};
          
          DynamicJsonDocument chan1(1024);
                              chan1["key"] = "wifi_scan_end";
                              
           serializeJson(chan1, jsonBuffer, sizeof(jsonBuffer));
           
  return jsonBuffer;
}

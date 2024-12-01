void MQTT_send(){                          // Функция отправки данных по протоколу MQTT
   client.loop();
   long nows = millis();
  if (nows - lastMsg > 30000) {
    if(WiFi.status() == WL_CONNECTED) {
        MQTT_send_data("jsondata", JSON_DATA_MQTT());
     }else{
         wi_fi_connect();
     }
   lastMsg = nows; 
  }
 }
void MQTT_send_data(String topic, String data){
         String root = settings.mqtt_topic;
         String top  = root +"/"+ topic;
         String input = settings.mqtt_serv;
         int colonIndex = input.indexOf(':');
         String ipAddress;
         String port;
         IPAddress Remote_MQTT_IP;

        if (colonIndex != -1) {
             ipAddress = input.substring(0, colonIndex);
             port = input.substring(colonIndex + 1);
             WiFi.hostByName(ipAddress.c_str(), Remote_MQTT_IP);
          }
      
      client.setServer(Remote_MQTT_IP, port.toInt());
           if(client.connected()){                                                                                    // Вторичная отправка данных при подключенном брокере 
          count_rf = 0;
          send_mqtt(top, data);
           }else{
              count_rf++;
              String ch_id = get_chip_id();
              if (client.connect(ch_id.c_str(), settings.mqtt_user, settings.mqtt_passw)){                             // Первичная отправка данных, выполняестя попытка подключения к брокеру 
                    send_mqtt(top, data);          
                }else{
                  if(count_rf > 2){                                                                                    // Если были неудачные попытки подключения, то переподключаем Wi-fi
                     WiFi.disconnect();
                     WiFi.begin(settings.mySSID, settings.myPW);
                     count_rf = 0;
                }
            }
        }
     
}

void send_mqtt(String tops, String data){
                                                                        // Анонсируем объекты для Home Assistant [auto-discovery ]
           mqqt_d_annonce("TotalIN", "r_cur_in", "water", "L");         // Имя параметра | json ключ | класс устройства | Единица измерения
           mqqt_d_annonce("TotalOUT", "r_cur_out", "water", "L");
           mqqt_d_annonce("Total5mIn", "lp5mIn", "water", "L");
           mqqt_d_annonce("Total5mOut", "lp5mOut", "water", "L");
           mqqt_d_annonce("WPress", "prs", "pressure", "bar");
           mqqt_d_annonce("WQual", "tds", "carbon_dioxide", "ppm");
           mqqt_d_annonce("Effc", "effic", "battery", "%");
           mqqt_d_annonce("Res_P", "res_P", "battery", "%");
           mqqt_d_annonce("Res_M", "res_M", "battery", "%");
           mqqt_d_annonce("Res_PO", "res_PO", "battery", "%");
           mqqt_d_annonce("FlowIN", "flow_in", "water", "L/min");
           mqqt_d_annonce("FlowOUT", "flow_out", "water", "L/min");

    client.publish(tops.c_str(), data.c_str());
}

void mqqt_d_annonce(String namec, String cn, String devclass, String unit){

          String top = String(settings.mqtt_topic) +"/jsondata";
          char jsonBuffer[1024] = {0};
          
          DynamicJsonDocument chan1(1024);
                              chan1["name"]                 = namec;
                              chan1["device_class"]         = devclass;
                              chan1["state_topic"]          = top;
                              chan1["unit_of_measurement"]  = unit;
                              chan1["value_template"]       = "{{ value_json."+cn+" }}";
                              
           serializeJson(chan1, jsonBuffer, sizeof(jsonBuffer));
          // Serial.println(jsonBuffer);
           String top_to = "homeassistant/sensor/osmos_"+cn+"/config";
           client.publish(top_to.c_str(), jsonBuffer, true);
}
 

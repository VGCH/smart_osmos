void get_data_bt(String text) {  // получение параметров от мобильного приложения
  if (text.length() > 1) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, text);
   // Serial.println(text);
    String key = doc["key"];
    if (key == "get_config") {  // Выполняем условие в зависимости от ключа
      datasend = false;
      change_charact(JSON_config_DATA());
      //datasend = true;

    } else if (key == "save") {  // Сохранение конфига
      String wifissid   = doc["wifissid"];
      String wifipass   = doc["wifipass"];
      String mqqtserver = doc["mqttserver"];
      String mqqtpass   = doc["mqttpass"];
      String mqqtuser   = doc["mqttuser"];
      String mqqttipic  = doc["mqtttopic"];

      strncpy(settings.mySSID, wifissid.c_str(), MAX_STRING_LENGTH);
      strncpy(settings.myPW, wifipass.c_str(), MAX_STRING_LENGTH);
      strncpy(settings.mqtt_serv, mqqtserver.c_str(), MAX_STRING_LENGTH);
      strncpy(settings.mqtt_user, mqqtuser.c_str(), MAX_STRING_LENGTH);
      strncpy(settings.mqtt_passw, mqqtpass.c_str(), MAX_STRING_LENGTH);
      strncpy(settings.mqtt_topic, mqqttipic.c_str(), MAX_STRING_LENGTH);
      save_config();
      wi_fi_connect();

    } else if(key == "c_config"){
      String c_in      = doc["c_in"];
      String c_off     = doc["c_off"];
      String prefiltr  = doc["prefiltr"];
      String membrana  = doc["membrana"];
      String postfiltr = doc["postfiltr"];
      settings.counter_coeff_in  = c_in.toInt();
      settings.counter_coeff_out = c_off.toInt();
      settings.prefiltr          = prefiltr.toInt();
      settings.membrana          = membrana.toInt();
      settings.postfiltr         = postfiltr.toInt();
      save_config();

    } else if (key == "c_reset"){
      
        if (doc["r_totalIN_P"] == "R"){
            settings.totalIN_P = 0;
         }
       if (doc["r_totalIN_M"] == "R"){
            settings.totalIN_M = 0;
       } 
       if (doc["r_totalIN_PO"] == "R"){
           settings.totalIN_PO = 0;
       }
       if (doc["r_totalIN_OUT"] == "R"){
           settings.totalIN  = 0;
           settings.totalOUT = 0;        
       }
           save_config();
       
    } else if (key == "wifi") {  // Сохранение точки доступа
      wi_fi_scan();

    } else if (key == "get_data") {
       String en_data = doc["en"];
         if(en_data == "On"){
            datasend = true;
         }else{
            datasend = false;
         }

    }else if (key == "update") {
       String en_data = doc["en"];
       String en_pass = doc["psw"];
              en_pass.trim();
         if(en_data == "On" && en_pass == ota_en_password){
            ota_update = true;
         }else{
            ota_update = false;
         }

    }else if (key == "wifien") {
      String en_dis = doc["en"];
         if (en_dis == "On") {
             settings.wifi_en = true;
             save_config();
             wi_fi_connect();

      } else if (en_dis == "Off") {
        settings.wifi_en = false;
        save_config();
        wi_fi_connect();
      }
    }
  }
}

void wi_fi_connect() {
  if (settings.wifi_en) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    WiFi.begin(settings.mySSID, settings.myPW);
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
      String  ch_id = get_chip_id();
      MDNS.begin(ch_id.c_str());
      server.on("/", HTTP_GET, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/html", FPSTR(serverIndex));
    });
    server.on(
      "/update", HTTP_POST,
      []() {
        server.sendHeader("Connection", "close");
        server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        reboot_t = millis();
        stat_reboot = true;
      },
      []() {
        HTTPUpload &upload = server.upload();
        if (upload.status == UPLOAD_FILE_START) {
          Serial.setDebugOutput(true);
          Serial.printf("Update: %s\n", upload.filename.c_str());
          save_config();
          if (!Update.begin()) {  //start with max available size
            Update.printError(Serial);
          }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            //Update.printError(error);
            server.send(200, "text/html", "<h1>Ошибка обновления!</h1>");
          }
        } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) {  //true to set the size to the current progress
            server.send(200, "text/html", "<h1>Обновление прошивки успешно завершено!<br>");
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
          } else {
            //Update.printError(error);
            server.send(200, "text/html", "<h1>Ошибка обновления!</h1>");
          }
          Serial.setDebugOutput(false);
        } else {
         // server.send(200, "Update Failed Unexpectedly (likely broken connection): "+String(upload.status));
        }
      }
    );
    server.begin();
    MDNS.addService("http", "tcp", 80);

    Serial.printf("Ready! Open http://%s.local in your browser\n", ch_id.c_str());
  } else {
    Serial.println("WiFi Failed");
  }
  } else {
    WiFi.mode(WIFI_OFF);
    WiFi.disconnect();
  }
}

void mesuring_data() {  // Переодическая обработка данных счетчика

  if (millis() - timeSensor > 300000) {                // Каждые 5 минут
    if(settings.counter_coeff_in != 0 && settings.counter_coeff_out != 0){
       lp5mIn  = (float)countRIN/settings.counter_coeff_in;     // Рассчитываем расход с учетом веса испульса счетчика
       lp5mOut = (float)countROUT/settings.counter_coeff_out;   // Рассчитываем расход с учетом веса испульса счетчика
    }
    settings.totalIN    += lp5mIn;     // Накопление расхода входного счетчика
    settings.totalIN_P  += lp5mIn;     // Накопление расхода входного счетчика
    settings.totalIN_M  += lp5mIn;     // Накопление расхода входного счетчика
    settings.totalIN_PO += lp5mIn;     // Накопление расхода входного счетчика

    settings.totalOUT += lp5mOut;      // Накопление расхода выходного счетчика

    countRIN  = 0;                     // Обнуляем значение счетчиков
    countROUT = 0;

    timeSensor = millis();
  }

  if (millis() - timeModeSwitch2 > 43200000) {  // Сохраняем данные в EEPROM каждые 12 часов

    save_config();

    timeModeSwitch2 = millis();
  }
}

void led_stat() {  // Индикатор подключения
  if (!stat_connect) {
    if (millis() - timeModeSwitch > 500) {
      digitalWrite(LED, !digitalRead(LED));
      timeModeSwitch = millis();
    }
  } else {
    digitalWrite(LED, HIGH);
  }
}



void change_charact(String data) {  // Записываем данные в характеристику
 // long btime = millis();
//  if (btime - timeSend > 10 or timeSend == 0) {
    if (stat_connect) {
      pCharacteristic->setValue(data.c_str());
      pCharacteristic->notify();
    }
  //  timeSend = btime;
 // }
}

String get_chip_id() {
  char macValue[17];
  uint64_t mac = ESP.getEfuseMac();
  uint32_t hi = mac >> 32;
  uint32_t lo = mac;
  sprintf(macValue, "%08x%08x", hi, lo);
  return macValue;
}

void data_p_send() {                        // Переодическая отправка данных
  
  if(datasend){
    long btime = millis();
    if (btime - timeSend > 500) {
       change_charact(JSON_DATA());
       timeSend = btime;
     }
  }
}

void presure_data_get(){
      long btime = millis();
     
    if (btime - timePrs > 2100) {
      prs_sensor_data_s = prs_sensor_data(analogRead(PRS_in));    // Получение данных датчика давления
      TDS_Data_s = TDS_Data(analogRead(TDS));                     // Получение данных TDS сенсора
      timePrs = btime;
      
    }
  
}
void data_flow() {                        // Переодический подсчет потока и чтение значения аналогового сигнала TDS датчика
  
    long btime = millis();
     
    if (btime - timeFlow > 5000) {
      
      float dtime = (float)(btime - timeFlow)/1000;
      float time_s = 60/dtime;
      
      if(flowRIN != 0 && settings.counter_coeff_in != 0){
           flowIn  = (float)(flowRIN * time_s)/settings.counter_coeff_in;
           flowRIN = 0;
        }else{
           flowIn  = 0;
           flowRIN = 0;
         }
         
      if(flowROUT != 0 && settings.counter_coeff_out != 0){
            flowOut = (float)(flowROUT * time_s)/settings.counter_coeff_out;
            flowROUT = 0;
         }else{
            flowOut  = 0;
            flowROUT = 0;
         }
         
       timeFlow = btime;
     }
  
}
void reboot_dev_delay(){
  if(stat_reboot && (millis() - reboot_t) > 2000){
       ESP.restart();
  } 
}

String Wi_Fi_status(){
  String statuss = "disconnect";
      if(WiFi.status() == WL_CONNECTED) {
         statuss = "connect";
     }
     return statuss;
}

void web_update_loop(){
        if(WiFi.status() == WL_CONNECTED && ota_update) {
         server.handleClient();
         delay(2);
     }
}

String ip_address(){
  IPAddress ip = WiFi.localIP();
  String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
  return ipStr;
}

String MQTT_status(){
    String message = "disconnect";
    if(client.connected()){
         message = "connect";
      }
    return message;
}


float TDS_Data(int AnalogDATA){

  double wTemp= 25;
  float V_level= 2.3 / 3500.0;
  float rawEc = (float) AnalogDATA * V_level;       // Raw  data of EC
  float T_Cof = 1.0 + 0.02 * (wTemp - 25.0);        // Temperature Coefficient
  
  float ec_Val    = rawEc / T_Cof;                  // temperature and calibration compensation
  float tds_value = (133.42 * pow(ec_Val, 3) - 255.86 * ec_Val * ec_Val + 857.39 * ec_Val) * 0.5; 
  return tds_value;
  
}


float prs_sensor_data(int AnalogDATA_prs){
     float prs = 0;
     float V_level = 3.3 / 4095.0;
            prs = (float) AnalogDATA_prs * V_level;
            Serial.println(prs);
            prs = prs_rang(prs, 0.2, 3.15, 0.0, 5.0);
        
     return prs;
}

String millis2time(){                                       // преобразование миллисекунд в вид д/ч/м/с
  
         int times =(millis()/1000);
         int days =  (times/3600)/24;
         int timehour =(((times)  % 86400L) / 3600); // часы
        if ( ((times % 3600) / 60) < 10 ) {
         int timehour = 0;
               }
         int timeminuts=((times  % 3600) / 60); // минуты
         if ( (times % 60) < 10 ) {
         int timeminuts = 0;
             }
         int timeseconds=(times % 60); // секунды
       String Time= String(days)+":"+String(twoDigits(timehour))+":"+String(twoDigits(timeminuts))+":"+String(twoDigits(timeseconds));
       return Time;
     }

String twoDigits(int digits){
             if(digits < 10) {
          String i = '0'+String(digits);
          return i;
         }
          else {
        return String(digits);
         }
      }

int res_prs(int limit, unsigned long cur){
    int pr = 0;
    if(cur != 0 && limit != 0){
        pr = cur * 100 / limit;
     }
    return pr;
 }    

float effect(){
  float pr = 0.0;
    if(lp5mOut != 0 && lp5mIn != 0){
        pr = (float) 100 - (lp5mOut * 100 / lp5mIn);
        if(pr < 0){
          pr = 0.0;
        }
    }
    
    return pr;
 }  

 float prs_rang(float x, float in_min, float in_max, float out_min, float out_max){
   return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

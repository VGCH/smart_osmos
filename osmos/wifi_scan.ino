void wi_fi_scan(){
  datasend = false;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  NetworksWiFi.clear();
  int n = WiFi.scanNetworks();
  if (n != 0) {
     for (int i = 0; i < n; ++i) {
          char jsonBuffer[1024] = {0}; 
          DynamicJsonDocument chan1(1024);
                              chan1["key"]          = "scanwifi";
                              chan1["networks"]     = n;
                              chan1["queny"]        = i;
                              chan1["ssid"]         = String(WiFi.SSID(i));
                              chan1["rssi"]         = WiFi.RSSI(i);
                              chan1["channel"]      = WiFi.channel(i);
                              
       switch (WiFi.encryptionType(i)) {
            case WIFI_AUTH_OPEN:            chan1["encryption"] = "open"; break;
            case WIFI_AUTH_WEP:             chan1["encryption"] = "WEP";  break;
            case WIFI_AUTH_WPA_PSK:         chan1["encryption"] = "WPA";  break;
            case WIFI_AUTH_WPA2_PSK:        chan1["encryption"] = "WPA2"; break;
            case WIFI_AUTH_WPA_WPA2_PSK:    chan1["encryption"] = "WPA+WPA2"; break;
            case WIFI_AUTH_WPA2_ENTERPRISE: chan1["encryption"] = "WPA2-EAP"; break;
            case WIFI_AUTH_WPA3_PSK:        chan1["encryption"] = "WPA3";     break;
            case WIFI_AUTH_WPA2_WPA3_PSK:   chan1["encryption"] = "WPA2+WPA3"; break;
            case WIFI_AUTH_WAPI_PSK:        chan1["encryption"] = "WAPI"; break;
            default:                        chan1["encryption"] = "unknown";
      }
     serializeJson(chan1, jsonBuffer, sizeof(jsonBuffer));
     NetworksWiFi.push_back(String(jsonBuffer));                                   // Добавляем jsonBuffer в вектор NetworksWiFi
       //change_charact(jsonBuffer);
      
     
   }
  WiFi.scanDelete();
  WiFi.mode(WIFI_OFF);
  wi_fi_connect();
  count_net = 0;
  search_net = true;
 }
} 

void printNetworks() {
    if(search_net){
    long tback = millis();
    if (tback - timeS > 1000) {
      if(NetworksWiFi.size() > count_net){
        Serial.println("JSON " + String(count_net) + ": " + NetworksWiFi[count_net]);
           change_charact(NetworksWiFi[count_net]);
           count_net++;
     
      }else{
        change_charact(JSON_scan_end());
        timeS2++;
      }
     if(timeS2 >= 3) {
        datasend   = true;
        search_net = false;
        timeS2 = 0;
      }
       timeS = tback;
        }
    }
}

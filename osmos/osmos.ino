/* Микро ПО модуля для контроля параметров фильтра
 * 
 * CYBEREX TECH
 * 2024
 * 
 */
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <EEPROM.h>
#include <vector>
#include <ArduinoJson.h>
#include "WiFi.h"

#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "webd.h"
#include <PubSubClient.h>


WebServer server(80);                             // WEB OTA обновление прошивки

BLEServer *pServer = NULL;
BLECharacteristic *pCharacteristic = NULL;

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"


#define counterIN 14  // Вход
#define counterOUT 2  // Выход

#define LED 32    // Пин светодиода
#define TDS 34    // Пин сенсора качества воды
#define PRS_in 35 // Аналоговый вход для датчика давления

//TwoWire I2CBME = TwoWire(0);

unsigned int countRIN  = 0;  // Переменная в обработчике прерываний по подсчету импульсов
unsigned int countROUT = 0;  // Переменная в обработчике прерываний по подсчету импульсов

unsigned int flowRIN  = 0;  // Переменная в обработчике прерываний по подсчету импульсов
unsigned int flowROUT = 0;  // Переменная в обработчике прерываний по подсчету импульсов


int count_rf  = 0;
int count_net = 0;
bool search_net = false;
bool datasend   = false;
bool stat_reboot = false;
bool stat_connect;
long reboot_t = 0;
long lastMsg = 0;
long timeModeSwitch = 0;
long timeModeSwitch2 = 0;
long timeSensor = 0;
long timeSend = 0;
long timeS  = 0;
long timeFlow  = 0;
long timePrs  = 0;
int  timeS2 = 0;

float lp5mIn  = 0;
float lp5mOut = 0;
float flowIn  = 0;
float flowOut = 0;
float TDS_Data_s = 0.0;
float prs_sensor_data_s = 0.0;

bool   ota_update      = false;                                   // Статус OTA         
String ota_en_password = "OeN12345";                              // Пароль для активации функции OTA

std::vector<String> NetworksWiFi;                                 // Вектор для хранения сетей

WiFiClient espClient;
PubSubClient client(espClient);

//BLE калбеки

void get_data_s(String str) {
  get_data_bt(str);
}

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    stat_connect = true;                                         // Если устройство подключено
  }

  void onDisconnect(BLEServer *pServer) {
    pServer->startAdvertising();                                 // Перезапуск рекламы при отключении
    stat_connect = false;                                        // Если устройство отключено
    datasend = false;                                            // Откл передачу данных
  }
};

class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String rxValue = pCharacteristic->getValue();
    if (!rxValue.isEmpty()) {
      get_data_s(rxValue.c_str());                                // Обработка полученных данных
    }
  }
};


// Функции обработки прерывания

void IRAM_ATTR countOUT() {                                      // Обработчик прерывания для выходного счетчика
  countROUT++;
  flowROUT++;

}


void IRAM_ATTR countIN() {  // Обработчик прерывания для входного счетчика
  countRIN++;
  flowRIN++;

}

#define MAX_STRING_LENGTH 30  // Максимальное количество символов для хранения в конфигурации

struct {                      // Структура для хранения конфигурации устройства
  int statteeprom;
  int counter_coeff_in  = 0;
  int counter_coeff_out = 0;
  char mySSID[MAX_STRING_LENGTH];
  char myPW[MAX_STRING_LENGTH];
  char mqtt_serv[MAX_STRING_LENGTH];
  char mqtt_user[MAX_STRING_LENGTH];
  char mqtt_passw[MAX_STRING_LENGTH];
  char mqtt_topic[MAX_STRING_LENGTH];
  float totalIN    = 0;
  float totalIN_P  = 0;
  float totalIN_M  = 0;
  float totalIN_PO = 0;
  float totalOUT   = 0;
  unsigned int prefiltr  = 0;
  unsigned int membrana  = 0; 
  unsigned int postfiltr = 0;
  bool wifi_en;

} settings;


void setup() {
  Serial.begin(115200);
  EEPROM.begin(sizeof(settings));
  read_config();                                  // Чтение конфигурации из EEPROM
  check_clean();                                  // Очистка EEPROM при первом запуске
  pinMode(counterIN, INPUT);                      // Установка режима пина входного счетчика
  pinMode(counterOUT, INPUT);                     // Установка режима пина выходного счетчика
  pinMode(LED, OUTPUT);
  pinMode(0, OUTPUT);
  digitalWrite(0, LOW);
  digitalWrite(LED, HIGH);
  ble_ini();                                      // BLE Инициализация
  attachInterrupt(counterIN, countIN, RISING);    // Функция обработки внешнего прерывания входного счетчика
  attachInterrupt(counterOUT, countOUT, RISING);  // Функция обработки внешнего прерывания выходного счетчика
  wi_fi_connect();                                // Подключаемся к Wi-Fi
}



void loop() {
  mesuring_data();                          // Обработка измерений
  led_stat();                               // Индикация BLE подключения (мигает - откл, горит - подкл.)
  MQTT_send();                              // Функция отправки данных по MQTT 
  printNetworks();                          // Функция вывода результата сакнирования Wi-Fi сети
  data_p_send();
  data_flow();
  web_update_loop();
  reboot_dev_delay();
  presure_data_get();
}

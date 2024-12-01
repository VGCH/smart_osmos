void ble_ini() {
    BLEDevice::init("SMART OSMOS");                                        // Инициализация BLE с именем устройства
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
  
    BLEService *pService = pServer->createService(BLEUUID(SERVICE_UUID));
    pCharacteristic = pService->createCharacteristic(
                      BLEUUID(CHARACTERISTIC_UUID),
                      BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                    );

    pCharacteristic->addDescriptor(new BLE2902());
    pCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  
    pService->start();
  
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->start();
}

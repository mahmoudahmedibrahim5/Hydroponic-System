#include <Arduino.h>
#include "BLE.hpp"

/* BLE Variables */
BLEServer* server;
BLECharacteristic* thresholdCharacteristic;
BLECharacteristic* phValueCharacteristic;

/* PH Threshold Variables */
uint32_t txData = 0;
extern double currentThresoldValue;
extern double oldThresholdValue;

/* Connection Variables */
extern bool deviceConnected;
extern bool oldDeviceConnected;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* server) {
      deviceConnected = true;
      Serial.println("Client connected succesfully");
    };

    void onDisconnect(BLEServer* server) {
      deviceConnected = false;
      Serial.println("Client disconnceted");
    }
};

void BLE_voidInit(void)
{
  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  server = BLEDevice::createServer();
  server->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *service = server->createService(SERVICE_UUID);

  // Create a BLE Characteristics
  thresholdCharacteristic = service->createCharacteristic(
                      CHARACTERISTIC_THRESHOLD_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );
  phValueCharacteristic = service->createCharacteristic(
                      CHARACTERISTIC_PH_VALUE_UUID,
                      BLECharacteristic::PROPERTY_READ   |
                      BLECharacteristic::PROPERTY_WRITE  |
                      BLECharacteristic::PROPERTY_NOTIFY |
                      BLECharacteristic::PROPERTY_INDICATE
                    );

  // Create a BLE Descriptor
  BLE2902* pBLE2902 = new BLE2902();
  pBLE2902->setNotifications(true);
  thresholdCharacteristic->addDescriptor(pBLE2902);
  phValueCharacteristic->addDescriptor(pBLE2902);
  
  // Start the service
  service->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
  Serial.println("Waiting a client connection to notify...");
}

void BLE_voidOnConnect(void)
{
  oldDeviceConnected = deviceConnected;
}

void BLE_voidOnDisconnect(void)
{
  delay(500); // give the bluetooth stack the chance to get things ready
  server->startAdvertising(); // restart advertising
  Serial.println("start advertising");

  oldDeviceConnected = deviceConnected;
}

void BLE_voidRunning(void)
{
  if(oldThresholdValue != currentThresoldValue)
  {
    // Send the new threshold
    uint32_t txData = currentThresoldValue * 100;
    thresholdCharacteristic->setValue(txData);
    thresholdCharacteristic->notify();

    // Print the new value in the serial monitor to confirm
    Serial.print("Threshold Value is set to: ");
    Serial.println(currentThresoldValue);

    // Wait to avoid sending continously
    delay(1000);

    // Update the old value
    oldThresholdValue = currentThresoldValue;
  }
}

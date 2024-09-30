/*
    Video: https://www.youtube.com/watch?v=oCMOYS71NIU
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleNotify.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updated by chegewara

   Create a BLE server that, once we receive a connection, will send periodic notifications.
   The service advertises itself as: 4fafc201-1fb5-459e-8fcc-c5c9c331914b
   And has a characteristic of: beb5483e-36e1-4688-b7f5-ea07361b26a8

   The design of creating the BLE server is:
   1. Create a BLE Server
   2. Create a BLE Service
   3. Create a BLE Characteristic on the Service
   4. Create a BLE Descriptor on the characteristic
   5. Start the service.
   6. Start advertising.

   A connect hander associated with the server starts a background task that performs notification
   every couple of seconds.
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer* server = NULL;
BLECharacteristic* thresholdCharacteristic = NULL;
BLECharacteristic* phValueCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;


uint32_t txData = 0;
double currentThresoldValue = 1.23;
double oldThresholdValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID                  "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_THRESHOLD_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_PH_VALUE_UUID  "769b1d37-1c34-49bf-84b8-459759bbf211"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* server) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* server) {
      deviceConnected = false;
    }
};



void setup() {
  Serial.begin(115200);

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

  // https://www.bluetooth.com/specifications/gatt/viewer?attributeXmlFile=org.bluetooth.descriptor.gatt.client_characteristic_configuration.xml
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

void loop() {
    if(Serial.available()){
      String serialInput = Serial.readString();
      currentThresoldValue = serialInput.toDouble();
    }
    
    // notify changed value
    if (deviceConnected) 
    {
        if(oldThresholdValue != currentThresoldValue)
        {
          txData = currentThresoldValue * 100;
          thresholdCharacteristic->setValue(txData);
          Serial.print("Threshold Value is set to: ");
          Serial.println(currentThresoldValue);
          thresholdCharacteristic->notify();
          delay(1000);
          oldThresholdValue = currentThresoldValue;
        }
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        server->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }

    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
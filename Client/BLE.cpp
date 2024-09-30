#include <Arduino.h>
#include "BLE.hpp"

/* BLE Variables */
BLERemoteCharacteristic* thresholdCharacteristic;
BLERemoteCharacteristic* phValueCharacteristic;
BLEAdvertisedDevice* myDevice;

/* Connection variables */
extern boolean doConnect;
extern boolean connected;
extern boolean doScan;

/* PH Threshold Variables */
extern double thresoldValue;

/******************************************* Callback Functions *******************************************/
void notifyThresholdCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) 
{
  Serial.print("New Threshold Value has been set to: ");
  uint32_t rxData = (uint32_t)((pData[0]) | (pData[1] << 8) | (pData[2] << (2 * 8)) | (pData[3] << (3 * 8)));
  thresoldValue = rxData / 100.0;
  Serial.println(thresoldValue);
}

void notifyPhValueCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{

}

class MyClientCallback : public BLEClientCallbacks 
{
  void onConnect(BLEClient* pclient) {
    Serial.println("Connected");
  }

  void onDisconnect(BLEClient* pclient) {
    Serial.println("Disconnected");
    connected = false;
  }
};

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks 
{
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) 
  {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    // We have found a device, let us now see if it contains the service we are looking for.
    static BLEUUID serviceUUID(SERVICE_UUID);
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(serviceUUID)) 
    {
      Serial.println("It has the same UUID");
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
    } // Found our server
    else
    {
      Serial.println("Different ID");
    }
  } // onResult
}; // MyAdvertisedDeviceCallbacks


/******************************************* Functions Declaration *******************************************/
void BLE_voidInit(void)
{
  BLEDevice::init("");

  /* Retrieve a Scanner and set the callback we want to use to be informed when we
   * have detected a new device.  Specify that we want active scanning and start the
   * scan to run for 5 seconds.
   */
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  /* Wait for found the desired BLE server and connect to it */
  // while(!doConnect);

  // if (connectToServer())
  //   Serial.println("We are now connected to the BLE Server.");
  // else 
  //   Serial.println("We have failed to connect to the server; there is nothin more we will do.");
  // doConnect = false;
}

bool BLE_voidConnectToServer(void) 
{
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
    
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remote BLE Server.
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");
    pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
  
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(SERVICE_UUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(SERVICE_UUID);
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    thresholdCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_THRESHOLD_UUID);
    if (thresholdCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(CHARACTERISTIC_THRESHOLD_UUID);
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    phValueCharacteristic = pRemoteService->getCharacteristic(CHARACTERISTIC_PH_VALUE_UUID);
    if (phValueCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(CHARACTERISTIC_PH_VALUE_UUID);
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    if(thresholdCharacteristic->canNotify())
      thresholdCharacteristic->registerForNotify(notifyThresholdCallback);

    connected = true;
    return true;
}

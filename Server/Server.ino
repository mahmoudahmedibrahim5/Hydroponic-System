/* 
 * Author: Mahmoud Ahmed 
 * Date: 30 Sep, 2024
 *
 * Target: ESP32
 * Project name: Hydroponic System
 *
 * Description:
 * This code is written for the main ECU (Server)
 * It Receives the PH Valu from the control ECU (Client) and 
 *    1) Print it in the serial monitor
 *    2) Send it to the mobile application
 * It sends the threshold value which it's received from
 *    1) Serial Monitor
 *    2) Mobile Application
 * 
*/

#include "BLE.hpp"

/* Connection Variables */
bool deviceConnected = false;
bool oldDeviceConnected = false;

/* PH Threshold Variables */
double currentThresoldValue = 7;
double oldThresholdValue = 0;

void setup() {
  Serial.begin(115200);
  BLE_voidInit();
}

void loop() {
  /* Read New Threshold Value */
  if (Serial.available()) {
    String serialInput = Serial.readString();
    currentThresoldValue = serialInput.toDouble();
  }

  /* Device Connecting */
  if (deviceConnected)
    BLE_voidRunning();

  /* When device disconnects */
  if (!deviceConnected && oldDeviceConnected)
    BLE_voidOnDisconnect();

  /* When device connects */
  if (deviceConnected && !oldDeviceConnected)
    BLE_voidOnConnect();
}
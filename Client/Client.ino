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
#include "PH_Sensor.hpp"
#include "Pump.hpp"

/* Connection variables */
boolean doConnect = false; // This flag is on when the scan find the desired BLE server
boolean connected = false;
boolean doScan = false;

/* PH Threshold Variables */
double thresoldValue = 7.0;
double phValue = 0;

void setup() 
{
  Serial.begin(115200);
  BLE_voidInit();  
  PH_voidInit();
  PUMP_voidInit();
} 

void loop() 
{
  /* Reading PH Sensor */
  phValue = PH_doubleRead();

  if(phValue > thresoldValue)
    PUMP_voidSetSpeed(500);
  else
    PUMP_voidSetSpeed(0);

  /* We have scanned for and found the desired BLE Server */
  if (doConnect == true) 
  {
    if (BLE_boolConnectToServer()) 
      Serial.println("We are now connected to the BLE Server.");
    else 
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    doConnect = false;
  }

  /* If we are connected to a peer BLE Server */
  if (connected) 
  {
    BLE_voidRunnung();
  }
  else if(doScan)
  {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  delay(1000); 
}

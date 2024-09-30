#ifndef _BLE_HPP_
#define _BLE_HPP_

#include "BLEDevice.h"
//#include "BLEScan.h"

// Service and Characteristics IDs
#define SERVICE_UUID                  "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_THRESHOLD_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_PH_VALUE_UUID  "769b1d37-1c34-49bf-84b8-459759bbf211"

/* @brief: This function Connect to server when we find the desired one
 * Create the BLE Device (Client) 
 * Connect to the BLE Server
 * Obtain reference to the desired service in the server
 * Obtain reference to the desired characteristics in the service (threshold and phValue)
 * assign callback function for threshold characterstic
 * Make connected true
 * @return: None
 */
bool BLE_voidConnectToServer(void);

/* @brief: This function Initialize the BLE
 * Init the BLE Device
 * Start scanning searching for the desired device
 * @return: None
 */
void BLE_voidInit(void);

#endif

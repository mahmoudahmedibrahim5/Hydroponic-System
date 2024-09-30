#ifndef _BLE_HPP_
#define _BLE_HPP_

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Service and Characteristics IDs
#define SERVICE_UUID                  "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_THRESHOLD_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_PH_VALUE_UUID  "769b1d37-1c34-49bf-84b8-459759bbf211"

/* Functions Prototypes */

/* @brief: This function Initialize the BLE
 * Create the BLE Device 
 * Create the BLE Server
 * Create the BLE Service
 * Create a BLE Characteristics
 * Create a BLE Descriptor
 * Start the service
 * Start advertising
 * @return: None
 */
void BLE_voidInit(void);

/* @brief: This function is called when the device reconnect
 * currently it do nothing
 * Update the value of oldDeviceConnected 
 * @return: None
 */
void BLE_voidOnConnect(void);

/* @brief: This function is called when the device disconnect
 * Give the bluetooth stack the chance to get things ready
 * Restart Advertising
 * Update the value of oldDeviceConnected 
 * @return: None
 */
void BLE_voidOnDisconnect(void);

/* @brief:
 * Checks if new thresold value is set by comparing new and old threshold values
 * if new value is set
 *    - Send the new threshold
 *    - Print the new value in the serial monitor to confirm
 *    - Wait to avoid sending continously
 *    - Update the old value
 * @return: None
 */
void BLE_voidRunning(void);

#endif

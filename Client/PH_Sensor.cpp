#include <Arduino.h>
#include "PH_Sensor.hpp"

void PH_voidInit(void)
{
  pinMode(PH_SENSOR_PIN, INPUT);
  // May be we do calibration stuff here
}

double PH_doubleRead(void)
{
  uint32_t readVal = analogRead(PH_SENSOR_PIN);
  return (readVal * 14.0) / 4096.0;
}

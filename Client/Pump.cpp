#include <Arduino.h>
#include "Pump.hpp"

void PUMP_voidInit(void)
{
  pinMode(PUMP_PIN, OUTPUT);
}

void PUMP_voidSetSpeed(uint8_t speed)
{
  analogWrite(PUMP_PIN, speed);
}

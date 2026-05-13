#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

float ultrasonicPulse(int trig, int echo);
bool IRSensorState(int pin);

#endif
#include "sensors.h"

float ultrasonicPulse(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH);

  float distance = duration * 0.034 / 2;
  return distance;
}

bool IRSensorState(int pin) {
  int result = digitalRead(pin);
  if (result == HIGH) {
    return true;
  } else {
    return false;
  }
}
#include <Arduino.h>


// set const pin use case
const int trig_pin = A0;
const int echo_pin = A1;
const int motorL0 = 9;
const int motorL1 = 10;
const int motorR0 = 5;
const int motorR1 = 6;
const int RCenterIRSensor = 4;
const int LCenterIRSensor = 11;

enum class motor_state {
  Stop,
  Forward,
  Backward,
};

enum car_state {
  Stop,
  Forward,
  Backward,
  Leftturn,
  Rightturn,
};

// put function declarations here:
float ultrasonicPulse(int trig, int echo);

void LMotorCtl(motor_state state, int speed);
void RMotorCtl(motor_state state, int speed);

void carState(car_state, int speed);

bool IRSensorState(int pin);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  pinMode(motorL0, OUTPUT);
  pinMode(motorL1, OUTPUT);
  pinMode(motorR0, OUTPUT);
  pinMode(motorR1, OUTPUT);

  pinMode(RCenterIRSensor, INPUT);
  pinMode(LCenterIRSensor, INPUT);

  pinMode(13, OUTPUT);

  carState(Stop, 0);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (IRSensorState(RCenterIRSensor)) {
    carState(Rightturn, 120);
  } else if (IRSensorState(LCenterIRSensor)) {
    carState(Leftturn, 120);
  } else {
    carState(Forward, 120);
  }
  float distance = ultrasonicPulse(trig_pin, echo_pin);
  if (distance <= 20) {
    digitalWrite(13, HIGH);
  } else {
    digitalWrite(13, LOW);
  }
  delay(500);
}

// put function definitions here:
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

void LMotorCtl(motor_state state, int speed) {
  switch (state)
  {
  case motor_state::Stop:
    analogWrite(motorL0, 0);
    analogWrite(motorL1, 0);
    return;
  case motor_state::Forward:
    analogWrite(motorL0, speed);
    analogWrite(motorL1, 0);
    return;
  case motor_state::Backward:
    analogWrite(motorL0, 0);
    analogWrite(motorL1, speed);
    return;
  }
}

void RMotorCtl(motor_state state, int speed) {
  switch (state)
  {
  case motor_state::Stop:
    analogWrite(motorR0, 0);
    analogWrite(motorR1, 0);
    return;
  case motor_state::Forward:
    analogWrite(motorR0, speed);
    analogWrite(motorR1, 0);
    return;
  case motor_state::Backward:
    analogWrite(motorR0, 0);
    analogWrite(motorR1, speed);
    return;
  }
}

void carState(car_state state, int speed) {
  switch(state) {
    case Stop:
      LMotorCtl(motor_state::Stop, 0);
      RMotorCtl(motor_state::Stop, 0);
      return;
    case Forward:
      LMotorCtl(motor_state::Forward, speed);
      RMotorCtl(motor_state::Forward, speed);
      return;
    case Backward:
      LMotorCtl(motor_state::Backward, speed);
      RMotorCtl(motor_state::Backward, speed);
      return;
    case Leftturn:
      LMotorCtl(motor_state::Forward, speed);
      RMotorCtl(motor_state::Stop, 0);
      return;
    case Rightturn:
      LMotorCtl(motor_state::Stop, 0);
      RMotorCtl(motor_state::Forward, speed);
      return;
  }
}
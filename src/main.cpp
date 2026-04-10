#include <Arduino.h>


// set const pin use case
const int trig_pin = A0;
const int echo_pin = A1;
const int motorL0 = 9;
const int motorL1 = 10;
const int motorR0 = 5;
const int motorR1 = 6;
const int RCenterIRSensor = 4;
const int ROuterIRSensor = 3;
const int LCenterIRSensor = 11;
const int LOuterIRSensor = 12;

enum class motor_state {
  Stop,
  Forward,
  Backward,
};

// put function declarations here:
float ultrasonicPulse(int trig, int echo);

void LMotorCtl(motor_state state, int speed);
void RMotorCtl(motor_state state, int speed);

class carState {
  motor_state RState = motor_state::Stop;
  motor_state LState = motor_state::Stop;
  void CheckState(motor_state currentL, motor_state currentR) {
    if (currentL != LState) {
      Stop();
      delay(50);
      LState = currentL;
    }
    if (currentR != RState) {
      Stop();
      delay(50);
      RState = currentR;
    }
  }
  public:
    void Stop() {
      LMotorCtl(motor_state::Stop, 0);
      RMotorCtl(motor_state::Stop, 0);
    }
    void Forward(int speed = 200) {
      CheckState(motor_state::Forward, motor_state::Forward);
      LMotorCtl(motor_state::Forward, speed);
      RMotorCtl(motor_state::Forward, speed);
    }
    void Backward(int speed = 80) {
      CheckState(motor_state::Backward, motor_state::Backward);
      LMotorCtl(motor_state::Backward, speed);
      RMotorCtl(motor_state::Backward, speed);
    }
    void Leftturn() {
      CheckState(motor_state::Forward, motor_state::Forward);
      LMotorCtl(motor_state::Forward, 200);
      RMotorCtl(motor_state::Forward, 120);
    }
    void Rightturn() {
      CheckState(motor_state::Forward, motor_state::Forward);
      LMotorCtl(motor_state::Forward, 120);
      RMotorCtl(motor_state::Forward, 200);
    }
};

bool IRSensorState(int pin);


carState car;

void setup() {
  // put your setup code here, to run once:

  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  pinMode(motorL0, OUTPUT);
  pinMode(motorL1, OUTPUT);
  pinMode(motorR0, OUTPUT);
  pinMode(motorR1, OUTPUT);

  pinMode(RCenterIRSensor, INPUT);
  pinMode(LCenterIRSensor, INPUT);

  pinMode(13, OUTPUT);

  car.Stop();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if(digitalRead(LCenterIRSensor) != HIGH && digitalRead(RCenterIRSensor) != HIGH) {
    car.Backward(150);
  } else if(digitalRead(RCenterIRSensor) != HIGH) {
    car.Rightturn();
  } else if(digitalRead(LCenterIRSensor) != HIGH) {
    car.Leftturn();
  } else {
    car.Forward(235);
  }
  delay(10);
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


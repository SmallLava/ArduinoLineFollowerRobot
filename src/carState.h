#ifndef CARSTATE_H
#define CARSTATE_H

#include <Arduino.h>

// 馬達腳位設定
const int motorL0 = 9;
const int motorL1 = 10;
const int motorR0 = 5;
const int motorR1 = 6;

class carState {
  enum class motor_state {
    Stop,
    Forward,
    Backward,
  };

  motor_state RState = motor_state::Stop;
  motor_state LState = motor_state::Stop;

  void CheckState(motor_state currentL, motor_state currentR);
  
public:
  void Stop();
  void Forward(int speed = 200);
  void Backward(int speed = 80);
  void Leftturn();
  void Rightturn();

  void LMotorCtl(motor_state state, int speed);
  void RMotorCtl(motor_state state, int speed);
};

#endif
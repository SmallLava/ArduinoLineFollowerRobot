#include "carState.h"

void carState::CheckState(motor_state currentL, motor_state currentR) {
  if (currentL != LState || currentR != RState) {
    LMotorCtl(motor_state::Stop, 0);
    RMotorCtl(motor_state::Stop, 0);
    delay(10); // 縮短延遲以消除循跡抖動
    LState = currentL;
    RState = currentR;
  }
}

void carState::Stop() {
  CheckState(motor_state::Stop, motor_state::Stop);
  LMotorCtl(motor_state::Stop, 0);
  RMotorCtl(motor_state::Stop, 0);
}

void carState::Forward(int speed) {
  CheckState(motor_state::Forward, motor_state::Forward);
  LMotorCtl(motor_state::Forward, speed);
  RMotorCtl(motor_state::Forward, speed);
}

void carState::Backward(int speed) {
  CheckState(motor_state::Backward, motor_state::Backward);
  LMotorCtl(motor_state::Backward, speed);
  RMotorCtl(motor_state::Backward, speed);
}

void carState::Leftturn(int speed) {
  CheckState(motor_state::Forward, motor_state::Forward);
  LMotorCtl(motor_state::Forward, 0);
  RMotorCtl(motor_state::Forward, speed);
}

void carState::Rightturn(int speed) {
  CheckState(motor_state::Forward, motor_state::Forward);
  LMotorCtl(motor_state::Forward, speed);
  RMotorCtl(motor_state::Stop, 0);
}

void carState::SharpLeft(int speed) {
  CheckState(motor_state::Backward, motor_state::Forward);
  LMotorCtl(motor_state::Backward, speed); // 稍微提升原地旋轉速度
  RMotorCtl(motor_state::Forward, speed);
}

void carState::SharpRight(int speed) {
  CheckState(motor_state::Forward, motor_state::Backward);
  LMotorCtl(motor_state::Forward, speed);
  RMotorCtl(motor_state::Backward, speed); // 稍微提升原地旋轉速度
}

void carState::Keep() {
  CheckState(LState, RState);
  LMotorCtl(LState, (LState == motor_state::Stop) ? 0 : 200);
  RMotorCtl(RState, (RState == motor_state::Stop) ? 0 : 200);
}

void carState::LMotorCtl(carState::motor_state state, int speed) {
  switch (state) {
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

void carState::RMotorCtl(carState::motor_state state, int speed) {
  switch (state) {
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
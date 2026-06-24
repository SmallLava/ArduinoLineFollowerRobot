#include <Arduino.h>
#include "carState.h"
#include "sensors.h"


// set const pin use case
const int trig_pin = A0;
const int echo_pin = A1;
const int RCenterIRSensor = 4;
const int ROuterIRSensor = 3;
const int LCenterIRSensor = 11;
const int LOuterIRSensor = 12;

carState car = carState();

// 設定感測器邏輯：依據需求設定「黑線」的狀態為 HIGH 或 LOW
const int BLACK = HIGH;
const int WHITE = (BLACK == LOW) ? HIGH : LOW;

// 設定避障距離閾值 (公分)
const float OBSTACLE_DISTANCE = 15.0;

// 循跡狀態控制變數
bool lastAllBlack = false;
bool isTracking = true; // 預設直接開始循跡
unsigned long startTime = 0; // 記錄啟動時間


void setup() {
  // put your setup code here, to run once:

  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  pinMode(motorL0, OUTPUT);
  pinMode(motorL1, OUTPUT);
  pinMode(motorR0, OUTPUT);
  pinMode(motorR1, OUTPUT);

  pinMode(ROuterIRSensor, INPUT);
  pinMode(LOuterIRSensor, INPUT);
  pinMode(RCenterIRSensor, INPUT);
  pinMode(LCenterIRSensor, INPUT);

  car.Stop();
  delay(1000);
  startTime = millis(); // 記錄進入 loop 前的時間
}

void loop() {
  // 判斷四個感測器是否全為黑線 (碰到終點橫線)
  bool currentAllBlack = (digitalRead(LOuterIRSensor) == BLACK &&
                          digitalRead(LCenterIRSensor) == BLACK &&
                          digitalRead(RCenterIRSensor) == BLACK &&
                          digitalRead(ROuterIRSensor) == BLACK);

  // 停止邏輯：加入啟動保護與二次確認
  if (currentAllBlack && !lastAllBlack && (millis() - startTime > 2000)) {
    // 偵測到全黑後，微幅前進 50ms 以確認是否為真正的終點橫線
    car.Forward(150);
    delay(50); 
    if (digitalRead(LOuterIRSensor) == BLACK && digitalRead(ROuterIRSensor) == BLACK) {
      isTracking = false;      // 確定是終點橫線：停止
      car.Stop();
    }
  }
  lastAllBlack = currentAllBlack;

  // 測量前方距離 (為節省效能，僅在循跡時測量)
  float distance = 999.0;
  if (isTracking) {
    distance = ultrasonicPulse(trig_pin, echo_pin);
  }

  // 偵測到障礙物，開始避障動作
  if (isTracking && distance > 0 && distance < OBSTACLE_DISTANCE) {
    car.Stop();
    delay(400);

    // 1. 原地左轉偏離軌道
    car.SharpLeft(220);
    delay(350);

    // 2. 向前繞開障礙物
    car.Forward(200);
    delay(1000); 

    // 3. 原地右轉回到軌道
    car.Rightturn(220);
    delay(550); 

    // 4. 直走直到感測器再次壓到黑線 (加入超時機制)
    car.Forward(170);
    bool isBackOnTrack = false;
    while (!isBackOnTrack) {
      delay(10);
      isBackOnTrack = (digitalRead(LOuterIRSensor) == WHITE || digitalRead(ROuterIRSensor) == BLACK);
    }
    
    car.Forward(100);
    delay(200);
    car.SharpRight(100);
    delay(200);
  }
  // 循跡中且前方無障礙、未踩在全黑標線上時，才執行一般的循跡邏輯
  else if (isTracking && !currentAllBlack) {
    bool LO = (digitalRead(LOuterIRSensor) == BLACK);
    bool LC = (digitalRead(LCenterIRSensor) == BLACK);
    bool RC = (digitalRead(RCenterIRSensor) == BLACK);
    bool RO = (digitalRead(ROuterIRSensor) == BLACK);

    if (RC && LC) {
      if(RO) {
        car.Rightturn(220); // 右外壓線 -> 大幅向右修正
      } else if (LO) {
        car.Leftturn(220); // 左外壓線 -> 大幅向左修正
      } else {
        car.Keep();
      }
    } else if (LO) {
      car.SharpLeft(230); // 最左壓線 -> 大幅向左修正
    } else if (RO) {
      car.SharpRight(240); // 最右壓線 -> 大幅向右修正
    } else if (LC) {
      car.Leftturn(230); // 中左壓線 -> 向左修正
    } else if (RC) {
      car.Rightturn(230); // 中右壓線 -> 向右修正
    } else {
      car.Forward(210); 
    }
  } else if (!isTracking) {
    car.Stop();
  }
  delay(10);
}

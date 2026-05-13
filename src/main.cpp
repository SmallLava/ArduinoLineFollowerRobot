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
const int BLACK = LOW;
const int WHITE = (BLACK == LOW) ? HIGH : LOW;

// 設定避障距離閾值 (公分)
const float OBSTACLE_DISTANCE = 15.0;

// 循跡狀態控制變數
int allBlackCount = 0;
bool lastAllBlack = false;
bool isTracking = false;


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

  pinMode(13, OUTPUT);

  car.Stop();
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  // 判斷四個感測器是否全為黑線 (碰到起終點黑線)
  bool currentAllBlack = (digitalRead(LOuterIRSensor) == BLACK &&
                          digitalRead(LCenterIRSensor) == BLACK &&
                          digitalRead(RCenterIRSensor) == BLACK &&
                          digitalRead(ROuterIRSensor) == BLACK);

  // 邊緣觸發檢測 (偵測剛踏上全黑的瞬間)
  if (currentAllBlack && !lastAllBlack) {
    allBlackCount++;
    if (allBlackCount == 1) {
      isTracking = true;       // 第一次全黑：開始循跡
      car.Forward(200);        // 稍微往前走確保跨過起點線，避免原邏輯誤判倒退
      delay(200);
    } else if (allBlackCount == 2) {
      isTracking = false;      // 第二次全黑：結束
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
    delay(500);

    // 1. 右轉偏離軌道
    car.Rightturn();
    delay(500); // 右轉時間 (依實體車轉向速度微調，大約轉 45~90 度)

    // 2. 向前繞開障礙物
    car.Forward(200);
    delay(1000); // 繞開的時間 (依實車狀況與障礙物大小微調)

    // 3. 左轉使車身斜向朝原本的軌道
    car.Leftturn();
    delay(500); // 左轉轉回軌道方向的時間

    // 4. 直走直到感測器再次壓到黑線
    car.Forward(150);
    while (digitalRead(LCenterIRSensor) == WHITE && digitalRead(RCenterIRSensor) == WHITE) {
      delay(10); // 持續直走，直到任一中間感測器看到黑線才跳出迴圈
    }
  }
  // 循跡中且前方無障礙、未踩在全黑標線上時，才執行一般的循跡邏輯
  else if (isTracking && !currentAllBlack) {
    if(digitalRead(LCenterIRSensor) == WHITE && digitalRead(RCenterIRSensor) == WHITE) {
      delay(500);
      if(digitalRead(LCenterIRSensor) == WHITE && digitalRead(RCenterIRSensor) == WHITE) {
        car.Backward(160);
      }
    } else if(digitalRead(RCenterIRSensor) == BLACK && digitalRead(LCenterIRSensor) == WHITE) {
      car.Rightturn();
    } else if(digitalRead(LCenterIRSensor) == BLACK && digitalRead(RCenterIRSensor) == WHITE) {
      car.Leftturn();
    } else {
      car.Forward(235);
    }
  } else if (!isTracking) {
    // 未開始或已結束時保持停止
    car.Stop();
  }
  delay(10);
}

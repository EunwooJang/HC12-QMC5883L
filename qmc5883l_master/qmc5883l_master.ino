#include "qmc5883l_multi.h"
#include <SoftwareSerial.h>

// 슬레이브 개수 설정
#define MAGNETIC_SLAVE_AMOUNT 5

// HC-12 핀 설정 (RX, TX)
SoftwareSerial hc12(D2, D1); // HC-12 모듈 연결 핀 (TX, RX)

// QMC5883LMulti 객체 생성
QMC5883LMulti compassMulti(MAGNETIC_SLAVE_AMOUNT);

void setup() {
  Serial.begin(115200);
  hc12.begin(9600); // HC-12 통신 시작
}

void loop() {
  char* data = compassMulti.getAllSensorData();

  // 수집된 데이터를 출력
  for (int i = 0; i < SLAVE_AMOUNT; i++) {
    Serial.print("Slave ");
    Serial.print(i + 1);
    Serial.print(": ");
    for (int j = 0; j < 6; j++) {
      Serial.print(data[i * 6 + j], HEX);
      Serial.print(" ");
    }
    Serial.println();
  }

  delay(3000); // 3초 대기 후 다음 데이터 요청
}

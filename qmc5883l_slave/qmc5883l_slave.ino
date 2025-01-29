#include "qmc5883l_edit.h"

// HC-12 핀 설정: Serial 포트를 HC-12에 직접 연결
#define HC12 Serial

// QMC5883L 센서 핀 설정 및 객체 생성
QMC5883LEdit compass;

// 슬레이브 ID 설정
const uint8_t SLAVE_ID = 1; // 이 값은 반드시 변경 가능해야 함

// 고정된 헤더를 저장할 배열
char fixedHeaderData[5]; // 동적으로 설정되는 헤더

// 이전 측정 데이터를 저장할 변수
char lastSentData[10]; // 4바이트 헤더 + 6바이트 센서 데이터

void setup() {
  HC12.begin(9600); // HC-12 시리얼 통신 시작

  // QMC5883L 초기화
  compass.init();

  // 동적으로 헤더 초기화
  snprintf(fixedHeaderData, sizeof(fixedHeaderData), "D%dMD", SLAVE_ID);

  // 이전 전송 데이터 초기화
  memset(lastSentData, 0, sizeof(lastSentData));
}

void loop() {
  // HC-12로부터 명령 수신 대기
  if (HC12.available() == 4) {
    char command[5];
    HC12.readBytes(command, 4);
    command[4] = '\0';

    // 데이터 요구 명령어 처리 (e.g., S1MD for SLAVE_ID = 1)
    char expectedCommand[5];
    snprintf(expectedCommand, sizeof(expectedCommand), "S%dMD", SLAVE_ID);
    if (strcmp(command, expectedCommand) == 0) {
      sendSensorData();
    }
    // 이전 데이터 재전송 명령어 처리 (e.g., S1MU for SLAVE_ID = 1)
    snprintf(expectedCommand, sizeof(expectedCommand), "S%dMU", SLAVE_ID);
    if (strcmp(command, expectedCommand) == 0) {
      resendLastData();
    }
  }
	// read로 쌓인 버퍼 초기화
	while (HC12.available()) {
		HC12.read();
	}
}

// 센서 데이터를 읽고 전송
void sendSensorData() {
  byte rawData[6]; // 자기장 센서의 RAW 데이터를 저장할 배열
  memset(rawData, 0, sizeof(rawData));

  // QMC5883L에서 RAW 데이터 읽기
  compass.readRawData(rawData);

  // 동적으로 설정된 헤더 전송
  memcpy(lastSentData, fixedHeaderData, 4); // 헤더 저장
  memcpy(&lastSentData[4], rawData, 6);     // 센서 데이터 저장

  // HC-12로 데이터 전송
  HC12.write(lastSentData, sizeof(lastSentData));
}

// 이전 데이터를 재전송
void resendLastData() {
  HC12.write(lastSentData, sizeof(lastSentData));
}

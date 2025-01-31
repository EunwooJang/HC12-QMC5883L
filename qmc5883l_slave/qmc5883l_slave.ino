#include "qmc5883l_edit.h"

#define SLAVE_ID 1  
#define SENSOR_TYPE 'M'  

// QMC5883L 센서 핀 설정 및 객체 생성
QMC5883LEdit compass;

char command1[5];
char command2[5];

// 고정된 헤더를 저장할 배열
char fixedHeaderData[5]; // 동적으로 설정되는 헤더

// 이전 측정 데이터를 저장할 변수
char lastSentData[10]; // 4바이트 헤더 + 6바이트 센서 데이터

void setup() {
	Serial1.begin(9600); // HC-12 시리얼 통신 시작
	Serial.begin(9600);
	
	// 미리 명령어 문자열 생성
  	snprintf(command1, sizeof(command1), "S%d%cD", SLAVE_ID, SENSOR_TYPE);
  	snprintf(command2, sizeof(command2), "S%d%cU", SLAVE_ID, SENSOR_TYPE);

  	// QMC5883L 초기화
  	compass.init();

  	// 동적으로 헤더 초기화
  	snprintf(fixedHeaderData, sizeof(fixedHeaderData), "D%dMD", SLAVE_ID);

  	// 이전 전송 데이터 초기화
  	memset(lastSentData, 0, sizeof(lastSentData));
}

void loop() {
  if (Serial1.available() > 0) {  // 데이터 수신 시작 감지
    unsigned long startTime = millis(); // 시작 시간 저장

    // 최대 10ms 동안 기다리면서 4바이트 도착 여부 확인
    while ((millis() - startTime) < 10) {
      if (Serial1.available() >= 4) {  // 4바이트가 도착하면 처리
        char command[4];
        Serial1.readBytes(command, 4);

        if (memcmp(command, command1, 4) == 0) {
            sendSensorData();
        } else if (memcmp(command, command2, 4) == 0) {
            resendLastData();
        }
				
	return;
    }
}

    // 10ms가 지나도 4바이트가 안 모이면 버퍼 초기화
    while (Serial1.available()) {
      Serial1.read();
    }
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
  Serial1.write(lastSentData, sizeof(lastSentData));
}

// 이전 데이터를 재전송
void resendLastData() {
  Serial1.write(lastSentData, sizeof(lastSentData));
}

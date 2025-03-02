#include "qmc5883l_edit.h"

// 3개의 센서 핀 설정 및 객체 생성
#define SENSOR1_SDA 2
#define SENSOR1_SCL 3
#define SENSOR2_SDA 4
#define SENSOR2_SCL 5
#define SENSOR3_SDA 6
#define SENSOR3_SCL 7

SoftwareWire myWire1(SENSOR1_SDA, SENSOR1_SCL);
SoftwareWire myWire2(SENSOR2_SDA, SENSOR2_SCL);
SoftwareWire myWire3(SENSOR3_SDA, SENSOR3_SCL);

QMC5883LEdit compass1(&myWire1);
QMC5883LEdit compass2(&myWire2);
QMC5883LEdit compass3(&myWire3);

void setup() {
    Serial.begin(9600); // 시리얼 모니터 시작

    // 각 QMC5883L 센서 초기화
    compass1.init();
    compass2.init();
    compass3.init();
}

void loop() {
    int16_t rawData1[3], rawData2[3], rawData3[3];

    // 각 센서에서 RAW 데이터 읽기
    compass1.readRawData(reinterpret_cast<char*>(rawData1));
    compass2.readRawData(reinterpret_cast<char*>(rawData2));
    compass3.readRawData(reinterpret_cast<char*>(rawData3));

    // 값의 범위를 -8에서 +8로 변환
    float normData1[3], normData2[3], normData3[3];
    for (int i = 0; i < 3; i++) {
        normData1[i] = 800.0 * rawData1[i] / 32768.0;
        normData2[i] = 800.0 * rawData2[i] / 32768.0;
        normData3[i] = 800.0 * rawData3[i] / 32768.0;
    }

    // 벡터 크기 (Norm) 계산
    float normValue1 = sqrt(normData1[0] * normData1[0] + normData1[1] * normData1[1] + normData1[2] * normData1[2]);
    float normValue2 = sqrt(normData2[0] * normData2[0] + normData2[1] * normData2[1] + normData2[2] * normData2[2]);
    float normValue3 = sqrt(normData3[0] * normData3[0] + normData3[1] * normData3[1] + normData3[2] * normData3[2]);

    // 시리얼 모니터에 데이터 출력
    Serial.print("Sensor1: ");
    for (int i = 0; i < 3; i++) {
        Serial.print(normData1[i], 2);
        Serial.print(" ");
    }
    Serial.print("| Norm: ");
    Serial.println(normValue1, 2);

    Serial.print("Sensor2: ");
    for (int i = 0; i < 3; i++) {
        Serial.print(normData2[i], 2);
        Serial.print(" ");
    }
    Serial.print("| Norm: ");
    Serial.println(normValue2, 2);

    Serial.print("Sensor3: ");
    for (int i = 0; i < 3; i++) {
        Serial.print(normData3[i], 2);
        Serial.print(" ");
    }
    Serial.print("| Norm: ");
    Serial.println(normValue3, 2);
    
    Serial.println("--------------------------");
    delay(100); // 100ms 대기 후 반복
}

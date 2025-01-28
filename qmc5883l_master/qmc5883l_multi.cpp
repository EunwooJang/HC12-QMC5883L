#include "qmc5883l_multi.h"
#include <Arduino.h>

// 외부에서 선언된 HC12 참조
extern SoftwareSerial HC12;

QMC5883LMulti::QMC5883LMulti(uint8_t slaveAmount)
  : slaveAmount(slaveAmount) {
  combinedData = new char[6 * slaveAmount];
  memset(combinedData, 0, 6 * slaveAmount);
}

void QMC5883LMulti::begin() {
  // 필요시 추가적인 초기화 코드
}

char* QMC5883LMulti::getAllSensorData() {
  memset(combinedData, 0, 6 * slaveAmount);

  for (uint8_t i = 1; i <= slaveAmount; i++) {
    char buffer[10];
    memset(buffer, 0, sizeof(buffer));

    if (!requestSensorData(i, buffer)) {
      Serial.print("Slave ");
      Serial.print(i);
      Serial.println(" failed. Requesting resend...");
      if (!requestResendData(i, buffer)) {
        Serial.print("Slave ");
        Serial.print(i);
        Serial.println(" failed again.");
      }
    }

    // Append valid data to combinedData
    memcpy(&combinedData[(i - 1) * 6], buffer + 4, 6); // Skip header (4 bytes)
  }

  return combinedData;
}

bool QMC5883LMulti::requestSensorData(uint8_t slaveId, char* buffer) {
  char command[5];
  snprintf(command, 5, "S%dMD", slaveId);

  HC12.write(command);

  unsigned long startTime = millis();
  while ((millis() - startTime) < 200) { // Wait up to 200ms for response
    if (HC12.available() >= 10) {
      HC12.readBytes(buffer, 10);
      return validateReceivedData(buffer, slaveId);
    }
  }

  return false; // Timed out
}

bool QMC5883LMulti::requestResendData(uint8_t slaveId, char* buffer) {
  char command[5];
  snprintf(command, 5, "S%dMU", slaveId);

  HC12.write(command);

  unsigned long startTime = millis();
  while ((millis() - startTime) < 200) { // Wait up to 200ms for response
    if (HC12.available() >= 10) {
      HC12.readBytes(buffer, 10);
      return validateReceivedData(buffer, slaveId);
    }
  }

  return false; // Timed out
}

bool QMC5883LMulti::validateReceivedData(const char* data, uint8_t slaveId) {
  char expectedHeader[5];
  snprintf(expectedHeader, sizeof(expectedHeader), "D%dMD", slaveId);

  if (strncmp(data, expectedHeader, 4) != 0) {
    return false; // Header mismatch
  }

  return true;
}

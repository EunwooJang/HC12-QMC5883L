#include "qmc5883l_multi.h"
#include <Arduino.h>
#include <SoftwareSerial.h>

// 외부에서 선언된 HC12 참조
extern SoftwareSerial hc12;

QMC5883LMulti::QMC5883LMulti(uint8_t slaveAmount)
  : slaveAmount(slaveAmount) {
  combinedData = new char[6 * slaveAmount];
  memset(combinedData, 0, 6 * slaveAmount);
}


char* QMC5883LMulti::getAllSensorData() {
  memset(combinedData, 0, 6 * slaveAmount);

  for (uint8_t i = 1; i <= slaveAmount; i++) {
    
    char buffer[10];
    memset(buffer, 0, sizeof(buffer));

    if (!requestSensorData(i, buffer)) {
      if (!requestResendData(i, buffer)) {
        memset(&combinedData[(i - 1) * 6], 0, 6);
      } else {
        memcpy(&combinedData[(i - 1) * 6], buffer + 4, 6);
      }
    } else {
      memcpy(&combinedData[(i - 1) * 6], buffer + 4, 6);
    }
  }
  return combinedData;
}

bool QMC5883LMulti::requestSensorData(uint8_t slaveId, char* buffer) {
  char command[5];
  snprintf(command, 5, "S%dMD", slaveId);
  hc12.write(command);
	hc12.flush();

	unsigned long startTime = millis();
  while ((millis() - startTime) < 200) { // 200ms 대기
    if (hc12.available() >= 10) {
      hc12.readBytes(buffer, 10);
      return validateReceivedData(buffer, slaveId);
    }
  }
    
  while (hc12.available()) {
    hc12.read();
  }

return false; // 타임아웃
}

bool QMC5883LMulti::requestResendData(uint8_t slaveId, char* buffer) {
  char command[5];
  snprintf(command, 5, "S%dMU", slaveId);

  hc12.write(command);
  hc12.flush();

	unsigned long startTime = millis();
  while ((millis() - startTime) < 200) { // 200ms 대기
    if (hc12.available() >= 10) {
      hc12.readBytes(buffer, 10);
      return validateReceivedData(buffer, slaveId);
    }
  }
    
  while (hc12.available()) {
    hc12.read();
  }

return false; // 타임아웃
}


bool QMC5883LMulti::validateReceivedData(const char* data, uint8_t slaveId) {
  char expectedHeader[5];
  snprintf(expectedHeader, sizeof(expectedHeader), "D%dMD", slaveId);
  return strncmp(data, expectedHeader, 4) == 0;
}

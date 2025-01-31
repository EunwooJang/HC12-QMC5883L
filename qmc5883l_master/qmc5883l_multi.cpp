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

  hc12.write(command);
	
	if (hc12.available()) {
  		unsigned long startTime = millis();
  		while ((millis() - startTime) < 100) { // Wait up to 200ms for response
    		if (hc12.available() >= 10) {
      		hc12.readBytes(buffer, 10);
      		return validateReceivedData(buffer, slaveId);
    		}
  		}
		while (hc12.avaliable()) {
			hc12.read();		
		}
	}
  return false; // Timed out
}

bool QMC5883LMulti::requestResendData(uint8_t slaveId, char* buffer) {
  char command[5];
  snprintf(command, 5, "S%dMU", slaveId);

  hc12.write(command);
	
	if (HC12.available()) {
  		unsigned long startTime = millis();
  		while ((millis() - startTime) < 100) { // Wait up to 200ms for response
    		if (hc12.available() >= 10) {
      		hc12.readBytes(buffer, 10);
      		return validateReceivedData(buffer, slaveId);
    		}
  		}
		while (hc12.avaliable()) {
			hc12.read();		
		}
	}
  return false; // Timed out
}

bool QMC5883LMulti::validateReceivedData(const char* data, uint8_t slaveId) {
  char expectedHeader[5];
  snprintf(expectedHeader, sizeof(expectedHeader), "D%dMD", slaveId);
  return strncmp(data, expectedHeader, 4) == 0;
}

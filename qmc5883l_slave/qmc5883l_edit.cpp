#include "qmc5883l_edit.h"
#include <Wire.h>

QMC5883LEdit::QMC5883LEdit() {}

void QMC5883LEdit::init() {
    Wire.begin();
    _writeReg(0x0B, 0x01); // Control register 설정
    _writeReg(0x09, 0x1D); // Continuous Mode, ODR=50Hz, RNG=8G, OSR=512
}

void QMC5883LEdit::setADDR(byte b) {
    _ADDR = b;
}

void QMC5883LEdit::_writeReg(byte reg, byte val) {
    Wire.beginTransmission(_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

void QMC5883LEdit::readRawData(byte *rawData) {
    Wire.beginTransmission(_ADDR);
    Wire.write(0x00); // 데이터 레지스터 시작 주소
    int err = Wire.endTransmission();
    if (!err) {
        Wire.requestFrom(_ADDR, (byte)6); // 6바이트 요청 (X, Y, Z)
        for (int i = 0; i < 6; i++) {
            rawData[i] = Wire.read(); // 읽은 데이터를 배열에 저장
        }
    }
}

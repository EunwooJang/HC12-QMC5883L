#include "qmc5883l_edit.h"

QMC5883LEdit::QMC5883LEdit(SoftwareWire *wire) {
    _wire = wire;
}

void QMC5883LEdit::init() {
    _wire->begin();
    _writeReg(0x0B, 0x01); // 설정 레지스터 (Control Register 2)
    _writeReg(0x09, 0x1D); // 데이터 읽기 모드 설정 (Continuous Mode, 200Hz)
}

void QMC5883LEdit::readRawData(byte *rawData) {
    _wire->beginTransmission(_ADDR);
    _wire->write(0x00); // X축 LSB 데이터 주소
    _wire->endTransmission();
    _wire->requestFrom(_ADDR, (byte)6); // 6바이트 데이터 읽기

    for (int i = 0; i < 6; i++) {
        if (_wire->available()) {
            rawData[i] = _wire->read();
        } else {
            rawData[i] = 0; // 읽기 실패 시 0으로 초기화
        }
    }
}

void QMC5883LEdit::_writeReg(byte reg, byte val) {
    _wire->beginTransmission(_ADDR);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
}

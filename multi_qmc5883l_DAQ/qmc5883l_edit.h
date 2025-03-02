#ifndef QMC5883L_EDIT_H
#define QMC5883L_EDIT_H

#include "Arduino.h"
#include <SoftwareWire.h>

class QMC5883LEdit {
  public:
    QMC5883LEdit(SoftwareWire *wire);
    void init();                  // 센서 초기화
    void readRawData(byte *rawData); // RAW 데이터 읽기 (6바이트)

  private:
    void _writeReg(byte reg, byte val); // 레지스터에 값 쓰기
    byte _ADDR = 0x0D;            // QMC5883L 기본 I2C 주소
    SoftwareWire *_wire;
};

#endif
#ifndef _CurrentReader_H_
#define _CurrentReader_H_

#include "Arduino.h"

class CurrentReader {
  uint8_t pin;
  static constexpr const float MV_PER_A = 66.f;  // 30A version
  // static constexpr const float MV_PER_A = 185.f;  // 5A version
public:
  CurrentReader(uint8_t _pin) : pin(_pin) {
  }
  float mA() {
    return abs(analogRead(this->pin) / 1024.f - 0.5f) * 5000 / CurrentReader::MV_PER_A * 1000;
  }
};

#endif

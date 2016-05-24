#ifndef _CurrentReader_H_
#define _CurrentReader_H_

#include "Arduino.h"

class CurrentReader {
public:
  CurrentReader(uint8_t _pin) : pin(_pin) {
    analogReference(DEFAULT);
    pinMode(pin, INPUT);
  }

  inline uint32_t mA() {
    int32_t raw = this->raw() + this->raw() + this->raw() + this->raw() + this->raw();
    return abs((raw * 5000 * 100 / CurrentReader::MV_PER_A * 10 / 5) - 38750000) / 1023; // mid at 511.5
    // return abs((raw * 5000 * 100 / CurrentReader::MV_PER_A * 10 / 5) - 38712120) / 1023; // mid at 511
  }

  inline int raw() {
    return analogRead(this->pin);
  }

private:
  const uint8_t pin;
  static constexpr const int32_t MV_PER_A = 66;  // 30A version
  // static constexpr const int32_t MV_PER_A = 185;  // 5A version
};

#endif

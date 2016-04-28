#ifndef _Motor_H_
#define _Motor_H_

#include "Arduino.h"

class Motor {
private:
  uint8_t a, b, en;
public:
  Motor(uint8_t _a, uint8_t _b, uint8_t _en) : a(_a), b(_b), en(_en) {
    pinMode(this->a, OUTPUT);
    pinMode(this->b, OUTPUT);
    pinMode(this->en, OUTPUT);
    this->stop();
  }

  void stop() {
    digitalWrite(this->en, LOW);
  }

  void open(uint8_t speed) {
    digitalWrite(this->a, LOW);
    digitalWrite(this->b, HIGH);
    analogWrite(this->en, speed);
  }

  void close(uint8_t speed) {
    digitalWrite(this->a, HIGH);
    digitalWrite(this->b, LOW);
    analogWrite(this->en, speed);
  }

};

#endif

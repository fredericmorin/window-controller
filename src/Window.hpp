#ifndef _Window_H_
#define _Window_H_

#include "Arduino.h"

#include "Motor.hpp"
#include "CurrentReader.hpp"

static const int MAX_INITIAL_CURRENT = 200;
static const int CLOSING_CURRENT_THRESHOLD = 1000;
static const int CLOSING_TIMEOUT_IN_MS = 10000;
static const int OPENING_CURRENT_PROTECTION = 600;
static const int OPENING_DURATION_IN_MS = 4000;
static const float RAMP_UP_TIME_IN_MS = 800.f;

class Window {
  Motor* motor;
  CurrentReader* current;
public:
  enum State {
    UNKNOWN = 0,
    OPENED = 1,
    CLOSED = 2
  } state;

public:
  Window(Motor* _motor, CurrentReader* _current) : motor(_motor), current(_current), state(UNKNOWN) {
  }

  uint8_t check() {
    this->motor->stop();
    delay(MAX_INITIAL_CURRENT);
    float current_max_mA = 0;
    for (size_t i = 0; i < 50; i++) {
      current_max_mA = max(current_max_mA, this->current->mA());
      delay(1);
      if (current_max_mA > MAX_INITIAL_CURRENT) {
        printf("\nERROR !\n");
        printf("Initial current is over limit: %i\n", int(current_max_mA));
        return 1;
      }
    }
    return 0;
  }

  void open() {
    if (check() != 0)
      return;

    printf("Opening window...\n");

    uint32_t start = millis(), duration = 0;
    float current = this->current->mA();
    while(1) {
      current = this->current->mA();
      duration = millis() - start;

      printf("\r%4ld current:%4d\a", duration, int(current));

      if (  // are we done yet ?
        current > OPENING_CURRENT_PROTECTION ||
        duration > OPENING_DURATION_IN_MS
      ) {
        this->state = Window::OPENED;
        break;
      }

      // ramp up from 0 to 100% in TIME ms.
      float progress = duration / RAMP_UP_TIME_IN_MS;
      this->motor->open(constrain(progress * 255, 0, 255));
    }

    printf("\rdone.\n");
    this->motor->stop();
    delay(100);
}

  void close() {
    if (check() != 0)
      return;

    printf("Closing window...\n");

    uint32_t start = millis(), elapsed = 0;
    float current = this->current->mA();
    while(1) {
      current = this->current->mA();
      elapsed = millis() - start;

      printf("\r%4ld current:%4d\a", elapsed, int(current));

      if (current > CLOSING_CURRENT_THRESHOLD) {
        this->state = Window::CLOSED;
        break;
      }
      if (elapsed > CLOSING_TIMEOUT_IN_MS) {
        printf("\r\nError: Timeout\n");
        this->motor->stop();
        return;
      }

      // ramp up from 0 to 100% in TIME ms.
      float progress = elapsed / RAMP_UP_TIME_IN_MS;
      this->motor->close(constrain(progress * 255, 0, 255));
    }

    printf("\rdone.\n");
    this->motor->stop();
    delay(100);
  }

};

#endif

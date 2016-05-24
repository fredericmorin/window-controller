#ifndef _Window_H_
#define _Window_H_

#include "Arduino.h"

#include "Motor.hpp"
#include "CurrentReader.hpp"

static const  int32_t MAX_INITIAL_CURRENT        = 200;
static const  int32_t CLOSING_CURRENT_THRESHOLD  = 2000;
static const uint32_t CLOSING_TIMEOUT_IN_MS      = 10000;
static const  int32_t OPENING_CURRENT_PROTECTION = 1200;
static const uint32_t OPENING_DURATION_IN_MS     = 4000;
static const    float RAMP_UP_TIME_IN_MS         = 800;

#include <Adafruit_BLE_UART.h>
extern Adafruit_BLE_UART nrf;

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
    for (size_t i = 0; i < 50; i++) {
      int32_t current = this->current->mA();
      delay(1);
      if (current > MAX_INITIAL_CURRENT) {
        printf("\nERROR: over limit\n");
        printf("  initial current is over limit: %lu\n", current);
        return 1;
      }
    }
    return 0;
  }

  void open() {
    if (check() != 0)
      return;

    printf("opening window...\n");

    uint32_t start = millis();
    int32_t current;
    uint32_t duration;
    while(1) {
      uint32_t now = millis();
      duration = now - start;
      current = this->current->mA();

      nrf.pollACI();

      static uint32_t last_run = now - 100;
      if (now - last_run >= 100) {
        last_run = now;
        printf("%4lums %4ldma\n", duration, current);
      }

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
    printf("%4lums %4ldma\n", duration, current);

    this->motor->stop();
    delay(50);
    printf("done.\n");
  }

  void close() {
    if (check() != 0)
      return;

    printf("closing window...\n");

    uint32_t start = millis();
    int32_t current;
    uint32_t elapsed;
    while(1) {
      uint32_t now = millis();
      elapsed = now - start;
      current = this->current->mA();

      nrf.pollACI();

      static uint32_t last_run = now - 100;
      if (now - last_run >= 100) {
        last_run = now;
        printf("%4lums %4ldma\n", elapsed, current);
      }

      if (current > CLOSING_CURRENT_THRESHOLD) {
        this->state = Window::CLOSED;
        break;
      }
      if (elapsed > CLOSING_TIMEOUT_IN_MS) {
        printf("\r\nERROR: Timeout\n");
        this->motor->stop();
        return;
      }

      // ramp up from 0 to 100% in TIME ms.
      float progress = elapsed / RAMP_UP_TIME_IN_MS;
      this->motor->close(constrain(progress * 255, 0, 255));
    }
    printf("%4lums %4ldma\n", elapsed, current);

    this->motor->stop();
    delay(50);
    printf("done.\n");
  }

};

#endif

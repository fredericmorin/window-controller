#ifndef _Console_H_
#define _Console_H_

#include <Adafruit_SSD1306.h>

class Console : public Adafruit_SSD1306 {
 public:
  Console(int8_t DC, int8_t RST, int8_t CS);

  size_t console_put(uint8_t c);

private:
  void console_scroll();
};

#endif

#include "Console.hpp"

extern uint8_t buffer[SSD1306_LCDHEIGHT * SSD1306_LCDWIDTH / 8];

static FILE uartout = {0} ;
static Console* console = NULL;
static int console_putchar (char c, FILE *stream)
{
  console->console_put(c);
  return 0 ;
}

Console::Console(int8_t DC, int8_t RST, int8_t CS) : Adafruit_SSD1306(DC, RST, CS) {
  clearDisplay();
  setTextSize(1);
  setTextColor(WHITE, BLACK);

  console = this;
  fdev_setup_stream (&uartout, console_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &uartout ;
}

size_t Console::console_put(uint8_t c) {
  if(c == '\n') {
    console_scroll();
  } else if(c == '\r') {
    cursor_x = 0;
    fillRect(0, cursor_y, SSD1306_LCDWIDTH, textsize * 8, BLACK);
  } else if(c == '\a') {
    display();
  } else {
    if((cursor_x + textsize * 6) >= _width) { // Heading off edge?
      console_scroll();
    }
    drawChar(cursor_x, cursor_y, c, textcolor, textbgcolor, textsize);
    cursor_x += textsize * 6;
  }
  return 1;
}

void Console::console_scroll() {
    cursor_x = 0;
    if ((cursor_y + 2 * textsize * 8) > SSD1306_LCDHEIGHT) {
      for (size_t x = 0; x < SSD1306_LCDWIDTH; x++) {
        for (size_t y = 0; y < SSD1306_LCDHEIGHT/8 - 1; y++) {
          buffer[x+ y*SSD1306_LCDWIDTH] = buffer[x+ (y+1)*SSD1306_LCDWIDTH];
        }
      }
      fillRect(0, cursor_y, SSD1306_LCDWIDTH, textsize * 8, BLACK);
    } else {
      cursor_y += textsize * 8;
    }
    display();
}

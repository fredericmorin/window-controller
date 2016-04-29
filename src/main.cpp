
#include "Arduino.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Console.hpp"
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif
#define OLED_DC     6
#define OLED_CS     8
#define OLED_RESET  7
Console console(OLED_DC, OLED_RESET, OLED_CS);

#include "Motor.hpp"
Motor motor(5, 4, 3);
#include "CurrentReader.hpp"
CurrentReader current(0);
#include "Window.hpp"
Window window(&motor, &current);

void setup()
{
  pinMode(0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  console.begin(SSD1306_SWITCHCAPVCC);
  console.display();

  printf("initialisation\n");

  window.close();
  if (window.state == Window::UNKNOWN) {
    while(1);
  }

  printf("system ready.\n\n");
}

void loop()
{
  uint32_t now = millis();

  static uint32_t last_run1 = now;
  if (now - last_run1 > 500) {
    last_run1 = now;

    static uint8_t done = 0;
    if (!done) {
      printf("\r");
      window.open();
      done = 1;
    }

    static uint8_t on = 0;
    printf("\r%c\a", on ? '_' : ' ');
    on = !on;
  }
}

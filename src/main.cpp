#include "Arduino.h"

#include "Motor.hpp"
Motor motor(5, 4, 3);
#include "CurrentReader.hpp"
CurrentReader current(0);
#include "Window.hpp"
Window window(&motor, &current);

#include <Adafruit_BLE_UART.h>
#include <SPI.h>
#define ADAFRUITBLE_REQ 10
#define ADAFRUITBLE_RDY 2
#define ADAFRUITBLE_RST 9
Adafruit_BLE_UART nrf = Adafruit_BLE_UART(ADAFRUITBLE_REQ, ADAFRUITBLE_RDY, ADAFRUITBLE_RST);
// #include "ble.hpp"

#include <Button.h>        //https://github.com/JChristensen/Button
Button btnopen(6, 1, 1, 20);    //Declare the button
Button btnclose(7, 1, 1, 20);    //Declare the button

volatile uint8_t nrfready = false;
void aciCallback(aci_evt_opcode_t event)
{
  switch(event)
  {
    case ACI_EVT_DEVICE_STARTED:
      printf("Advertising started\n");
      nrfready = true;
      break;
    case ACI_EVT_CONNECTED:
      printf("Connected!\n");
      break;
    case ACI_EVT_DISCONNECTED:
      printf("Disconnected or advertising timed out\n");
      break;
    default:
      break;
  }
}

void rxCallback(uint8_t *buffer, uint8_t len)
{
  uint8_t data[10];
  memcpy(data, buffer, min(len, 10));

  printf("Received %u bytes: ", len);
  for(int i=0; i<len; i++)
    printf("%u ", (char)data[i]);
  printf("\n");

  if (len == 2 && data[0] == 'o' && window.state == Window::CLOSED) {
    printf("open button pressed\n");
    nrf.println("opening...");
    window.open();
    nrf.println("done");
  }
  if (len == 2 && data[0] == 'c') {
    printf("close button pressed\n");
    nrf.println("closing...");
    window.close();
    nrf.println("done");
  }
}

static FILE uartout = {0} ;
static int console_putchar(char c, FILE *stream)
{
  Serial.write(c);
  return 0 ;
}

void setup()
{
  pinMode(0, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  fdev_setup_stream (&uartout, console_putchar, NULL, _FDEV_SETUP_WRITE);
  stdout = &uartout ;

  printf("\n\n\n\n\n\n\n\ninitialisation\n");
  window.close();
  if (window.state == Window::UNKNOWN) {
    while(1);
  }

  nrf.setRXcallback(rxCallback);
  nrf.setACIcallback(aciCallback);
  nrf.setDeviceName("window1"); /* 7 characters max! */
  nrf.begin();
  while (!nrfready)
    nrf.pollACI();
  printf("nrf ready\n");

  printf("system ready.\n\n");
}

void loop()
{
  uint32_t now = millis();

  nrf.pollACI();

  btnopen.read();
  if (btnopen.wasPressed() && window.state != Window::OPENED) {
    printf("open button pressed\n");
    window.open();
  }
  btnclose.read();
  if (btnclose.wasPressed()) {
    printf("close button pressed\n");
    window.close();
  }
  //
  // static uint32_t last_run2 = now;
  // if (now - last_run2 > 500) {
  //   last_run2 = now;
  //
  //   // static uint8_t done = 0;
  //   // if (!done) {
  //   //   printf("\r");
  //   //   window.open();
  //   //   done = 1;
  //   // }
  //
  //   static uint8_t on = 0;
  //   printf("\r%c\a", on ? '_' : ' ');
  //   on = !on;
  // }
}

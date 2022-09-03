#include <gpio.h>

const int PIN_RESISTOR_GPIO = 4;
const int PIN_FAN_GPIO = 5;

void fan_on(int intensity) {
  pinMode(PIN_FAN_GPIO, OUTPUT);
  softPwmCreate(PIN_FAN_GPIO, 0, 100);
  softPwmWrite(PIN_FAN_GPIO, intensity);
}

void fan_off() {
  pinMode(PIN_FAN_GPIO, OUTPUT);
  softPwmCreate(PIN_FAN_GPIO, 0, 100);
  softPwmWrite(PIN_FAN_GPIO, 0);
}

void resistor_on(int intensity) {
  pinMode(PIN_RESISTOR_GPIO, OUTPUT);
  softPwmCreate(PIN_RESISTOR_GPIO, 0, 100);
  softPwmWrite(PIN_RESISTOR_GPIO, intensity);
}

void resistor_off() {
  pinMode(PIN_RESISTOR_GPIO, OUTPUT);
  softPwmCreate(PIN_RESISTOR_GPIO, 0, 100);
  softPwmWrite(PIN_RESISTOR_GPIO, 0);
}

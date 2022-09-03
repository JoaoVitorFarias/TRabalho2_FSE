#ifndef GPIO_H_
#define GPIO_H_

#include <softPwm.h>
#include <wiringPi.h>

void fan_on(int intensity);
void fan_off();
void resistor_on(int intensity);
void resistor_off();


#endif /* GPIO_H_ */
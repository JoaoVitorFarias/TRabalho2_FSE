#ifndef MAIN_H_
#define MAIN_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#include <bme280.h>
#include <control_lcd.h>
#include <csv.h>
#include <gpio.h>
#include <pid.h>
#include <temp_bme280.h>
#include <uart.h>
#include <menu.h>


void set_up();
void start();
void reset();
void end();
void *menu();
void handle_menu();
void monitoring();
void define_info();      
void *control();
void time_control();
void handle_time();
void temperature_control();

#endif /* MAIN_H_ */
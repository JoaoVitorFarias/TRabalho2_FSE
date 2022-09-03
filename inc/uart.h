#ifndef UART_H_
#define UART_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>         
#include <fcntl.h>          
#include <termios.h>
#include <crc16.h>        

int init_uart();
void write_uart(int uart_filestream, char subcode);
void read_uart(int uart_filestream, unsigned char *rx_buffer);
float request_uart_temp(int uart_filestream, char subcode);
int request_uart_user(int uart_filestream);
void send_uart_int(int uart_filestream, char subcode, int value);
void send_uart_float(int uart_filestream, float value);
void send_uart_bool(int uart_filestream, bool value, char subcode);
void close_uart(int uart_filestream);


#endif /* UART_H_ */
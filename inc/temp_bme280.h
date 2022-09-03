#ifndef TEMP_BME280_H_
#define TEMP_BME280_H_

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <unistd.h>

#include <bme280.h>

struct bme280_dev init_bme();
void user_delay_ms(uint32_t period);
void print_sensor_data(struct bme280_data *comp_data);
int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data,
                      uint16_t len);
float request_temp(struct bme280_dev *dev);

#endif /* TEMP_BME280_H_ */
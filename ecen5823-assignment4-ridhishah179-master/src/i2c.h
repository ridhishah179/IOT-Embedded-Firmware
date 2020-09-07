/***********************************************************************************

* @i2c.h
* @author Ridhi Shah
* This file contains Header files for i2c functions
* @references: //siliconlabs.github.io/Gecko_SDK_Doc/efr32bg1/html/group__I2CSPM.html

************************************************************************************/

#include "em_i2c.h"
#include "gpio.h"
#include "i2cspm.h"
#include "em_cmu.h"

#define device_address (0x40) //i2c device address for Si7021

/*
 * @brief: This function Initializes the I2C SPM structure
 */
void i2c_init();

/*
 * @brief: This function Enables sensor
 */
void sensor_enable();

/*
 * @brief: This function Disables Sensor
 */
void sensor_disable();

/*
 * @brief: This function is used to write command to I2C
 */
void i2c_transfer_write();


/*
 * @brief: This function is used to read I2C data
 */
void i2c_transfer_read();


/*
 * @brief: This function is used to measure the temperature when an event occurs
 */
void measure_temp();

uint8_t i2c_read_data[2];
I2C_TransferReturn_TypeDef i2c_transfer_return_status;
I2C_TransferSeq_TypeDef seq;


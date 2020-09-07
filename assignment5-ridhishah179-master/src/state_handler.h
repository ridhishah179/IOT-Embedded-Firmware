
/***********************************************************************************

* @i2c.c
* @author Ridhi Shah
* This file contains header files and function declaration for state handler

************************************************************************************/
#include "i2c.h"
#include "letimer.h"
#include "log.h"
#include "cmu.h"
#include "em_cmu.h"
#include "sleep.h"
#include "native_gecko.h"

CORE_DECLARE_IRQ_STATE ;

//All states of state machine
enum states
{
	POWER_UP_STATE,
	I2C_WRITE_STATE,
	WAITFOR_I2C_WRITE_STATE,
	I2C_READ_STATE,
	WAITFOR_I2C_READ_STATE,
	POWER_OFF_STATE
};



enum states curr_state;
enum states next_state;


void state_machine();
/*
 * @brief: this function handles the power off state of the sensor
 */
void power_off();
/*
 * @brief: This function handles the powerup state
 */
void wait_for_powerup();

/*
 * @brief: This function handles the I2C write state and calls the write function
 */
void i2c_write();

/*
 * @brief: This function handles the state for I2C write to complete
 */
void i2c_write_complete();

/*
 * @brief: This function handles the i2c read state
 */
void i2c_read();

/*
 * @brief: This function handles the read complete state
 */
void i2c_read_complete();

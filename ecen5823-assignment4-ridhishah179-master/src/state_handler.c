/***********************************************************************************

* @state_handler.c
* @author Ridhi Shah
* This file contains functions for handling states of the state machine

************************************************************************************/

#include <src/state_handler.h>
#include <em_emu.h>

//handle state when sensor is disabled after read operation
void power_off()
{
	CORE_ENTER_CRITICAL();
	if(occured_event==disableevent)
	{
	occured_event = none;
	CORE_EXIT_CRITICAL();
	sensor_disable();
	next_state = POWER_UP_STATE; //switch to next state to wait for powerup
	}
	else
	{
		CORE_EXIT_CRITICAL();
	}

}

//wait for sensor powerup
void wait_for_powerup()
{
	CORE_ENTER_CRITICAL();
	if(occured_event == period_expired)
	{
	occured_event = none;
	CORE_EXIT_CRITICAL();
	sensor_enable();
	timerSetEventInMs(80);
	next_state = I2C_WRITE_STATE; //switch to next state for i2c write
	}
	else
	{
	CORE_EXIT_CRITICAL();
	}
}


//hanles the I2C write
void i2c_write()
{
	CORE_ENTER_CRITICAL();
	if(occured_event == delay)
	{
		occured_event = none;
		CORE_EXIT_CRITICAL();
		i2c_transfer_write();
		next_state = WAITFOR_I2C_WRITE_STATE; //switch to next state for i2c write to complete
	}
	else
	{
	CORE_EXIT_CRITICAL();
	}

}

//wait for write to complete
void i2c_write_complete()
{
	CORE_ENTER_CRITICAL();
	if(occured_event ==I2CtransferComplete )
	{
		occured_event = none;
		CORE_EXIT_CRITICAL();
		timerSetEventInMs(10);
		next_state = I2C_READ_STATE; //switch to next state for i2c read
	}
	else
	{
	CORE_EXIT_CRITICAL();
	}
}

//handle i2c read
void i2c_read()
{
	CORE_ENTER_CRITICAL();
	if(occured_event == delay)
	{

		occured_event = none;
		CORE_EXIT_CRITICAL();
		i2c_transfer_read();
		next_state = WAITFOR_I2C_READ_STATE;// switch to next state for i2c read to complete
	}
	else
	{
	CORE_EXIT_CRITICAL();
	}
}

//wait for read to complete
void i2c_read_complete()
{
	CORE_ENTER_CRITICAL();
	if(occured_event == I2CtransferComplete)
	{
		occured_event = none;
		CORE_EXIT_CRITICAL();
		measure_temp();
		next_state = POWER_OFF_STATE; //switch to next state for power off
		occured_event = disableevent;
		SLEEP_SleepBlockEnd(2);
		SLEEP_SleepBlockBegin(EM+(1));
	}
	else
	{
	CORE_EXIT_CRITICAL();
	}
}

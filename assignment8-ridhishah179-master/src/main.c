/***********************************************************************************

* @main.c
* @author Ridhi Shah
* @references: https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg1/html/group__SLEEP.html

************************************************************************************/
#include "gecko_configuration.h"
#include "em_emu.h"
#include "em_core.h"
#include "ble.h"

extern enum events occured_event;

int appMain(gecko_configuration_t *config)
{

  curr_state = POWER_UP_STATE;
  occured_event = none;

  //Initialize stack
  gecko_init(config);
  gpioInit();
  /*if(EM==1 || EM==2)
  {
	  int block_mode = (EM + (1));
	  SLEEP_SleepBlockBegin(block_mode);
  }*/

  cmu_setup();
  letimer_setup();
  i2c_init();
  logInit();
  displayInit();

  // Infinite loop
  while (1)
  {

	  struct gecko_cmd_packet* evt;


	 //if any even occurs, handle states
	if(occured_event != none)
	{
		switch(curr_state)
			  {

				case POWER_UP_STATE:
				{
					wait_for_powerup();
					break;
				}

				case I2C_WRITE_STATE:
				{
					i2c_write();
					 break;
				}

				case WAITFOR_I2C_WRITE_STATE:
				{
					i2c_write_complete();
					 break;
				}

				case I2C_READ_STATE:
				{
					i2c_read();
					break;
				}

				case WAITFOR_I2C_READ_STATE://read
				{
					i2c_read_complete();
					 break;
				}

				case POWER_OFF_STATE:
				{
					power_off();
					break;
				}
			  }

			//switch to next state after handling current state
			  if(curr_state != next_state)
			  {

				  logFlush();
				  curr_state = next_state;
			  }

	}


	  evt = gecko_wait_event();

	  gecko_ecen5823_update(evt);


  }


}



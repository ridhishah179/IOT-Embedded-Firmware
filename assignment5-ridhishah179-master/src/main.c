/***********************************************************************************

* @main.c
* @author Ridhi Shah
* @references: https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg1/html/group__SLEEP.html

************************************************************************************/
#include "gecko_configuration.h"
#include "native_gecko.h"
#include "em_emu.h"
#include "em_core.h"
#include "state_handler.h"
#include "gecko_ble_errors.h"

extern enum events occured_event;
int32_t rssi_value;
int32_t tx_power;
uint8_t connection_bt;



void gecko_ecen5823_update(struct gecko_cmd_packet* evt)
{
	 switch (BGLIB_MSG_ID(evt->header))
	 {
	      //This boot event is generated when the system boots up after reset.
	      case gecko_evt_system_boot_id: // Set advertising parameters.
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0, 400, 400, 0, 0));
	        /* Start general advertising and enable connections. */
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
	        break;

	      case gecko_evt_le_connection_opened_id:
	    	connection_bt=evt->data.evt_le_connection_opened.connection;
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_parameters(connection_bt,60,60,3,400));
	    	LOG_INFO("opened");
	    	  break;

	      case gecko_evt_le_connection_closed_id:
	          BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
	    	  break;

	      case gecko_evt_system_external_signal_id:

	    	  if(evt->data.evt_system_external_signal.extsignals  == 0 )
	    	  {
	    		  occured_event = none;
	    	  }
	    	  else if(evt->data.evt_system_external_signal.extsignals  == 1 )
	    	  {
	    	  	   occured_event = period_expired;
	    	  }
	    	  else if(evt->data.evt_system_external_signal.extsignals == 2 )
	    	  {
	    	  	    occured_event = delay;
	    	  }
	    	  else if(evt->data.evt_system_external_signal.extsignals  == 3 )
	    	  {
	    	  	    occured_event = I2CtransferComplete;
	    	  }
	    	  else if(evt->data.evt_system_external_signal.extsignals == 4 )
	    	  {
	    	  	   	occured_event = disableevent;
	    	  }
	    	  break;


	       case gecko_evt_le_connection_rssi_id:


	        	tx_power = 0;
	        	rssi_value = evt->data.evt_le_connection_rssi.rssi;
	        	LOG_INFO("RSSI VAL %d",rssi_value);
	        	if (rssi_value > -35 )
	        	{
	        		tx_power = -260;
	       	  	}
	        	else if (rssi_value > -45 && rssi_value <=(-35))
	        	{
	        		tx_power = -200;
	        	}
	        	else if (rssi_value > -55 && rssi_value <=(-45))
	        	{
	        		tx_power = -150;
	        	}
	        	else if (rssi_value >-65 && rssi_value<= (-55))
	        	{
	        		tx_power = -50;
	        	}else if (rssi_value >-75 && rssi_value <=(-65))
	        	{
	        		tx_power = 0;
	        	}
	        	else if (rssi_value >-85 && rssi_value <= (-75))
	        	{
	        		tx_power = 50;
	        	}else
	        	{
	        		tx_power = 80;
	        	}


	        	gecko_cmd_system_halt(1);
	        	gecko_cmd_system_set_tx_power(tx_power);
	        	gecko_cmd_system_halt(0);

	        	break;

	      default:
	    	  break;
	 }

}

int appMain(gecko_configuration_t *config)
{

  curr_state = POWER_UP_STATE;
  occured_event = none;

  //Initialize stack
  gecko_init(config);
  gpioInit();
  if(EM==1 || EM==2)
  {
	  int block_mode = (EM + (1));
	  SLEEP_SleepBlockBegin(block_mode);
  }

  cmu_setup();
  letimer_setup();
  i2c_init();
  logInit();

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
				 // LOG_INFO("Temp sensor transitioned from state %d to state %d",curr_state,next_state);
				  logFlush();
				  curr_state = next_state;
			  }

	}


	  evt = gecko_wait_event();

	  gecko_ecen5823_update(evt);


  }


}



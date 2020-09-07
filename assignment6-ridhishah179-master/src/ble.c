#include "ble.h"

void gecko_ecen5823_update(struct gecko_cmd_packet* evt)
{
	 switch (BGLIB_MSG_ID(evt->header))
	 {
	      //This boot event is generated when the system boots up after reset.
	      case gecko_evt_system_boot_id: // Set advertising parameters.
	    	displayPrintf(DISPLAY_ROW_NAME,"Server");
	    	disp_addr = gecko_cmd_system_get_bt_address();
	    	displayPrintf(DISPLAY_ROW_BTADDR,"%x:%x:%x:%x:%x:%x",disp_addr->address.addr[5],disp_addr->address.addr[4],disp_addr->address.addr[3],disp_addr->address.addr[2],disp_addr->address.addr[1],disp_addr->address.addr[0]);
	    	displayPrintf(DISPLAY_ROW_CONNECTION,"Advertising");
	    	displayUpdate();
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0, 400, 400, 0, 0));
	        /* Start general advertising and enable connections. */
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));

	        break;

	      case gecko_evt_le_connection_opened_id:
	    	displayPrintf(DISPLAY_ROW_CONNECTION,"Connected");
	    	displayUpdate();
	    	connection_bt=evt->data.evt_le_connection_opened.connection;
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_parameters(connection_bt,60,60,3,600));
	    	LOG_INFO("opened");
	    	  break;

	      case gecko_evt_le_connection_closed_id:
	    	  displayPrintf(DISPLAY_ROW_CONNECTION,"Advertising");
	    	  displayUpdate();
	    	  BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0, 400, 400, 0, 0));
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

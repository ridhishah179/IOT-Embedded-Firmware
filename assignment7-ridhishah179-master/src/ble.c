#include "ble.h"
#include "ble_device_type.h"

#if DEVICE_IS_BLE_SERVER
void gecko_ecen5823_update(struct gecko_cmd_packet* evt)
{
	 struct gecko_msg_system_get_bt_address_rsp_t *disp_addr;

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
	    	  displayPrintf(DISPLAY_ROW_TEMPVALUE,"");
	    	  displayUpdate();
	    	  BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(0, 400, 400, 0, 0));
	          BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
	          gecko_cmd_system_set_tx_power(0);
	    	  break;

	    //setting events on external signal
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
	        	//set appropriate tx power for rsssi value
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
#else

//function to convert Uint32 to float
float gattUint32ToFloat(const uint8_t *value_start_little_endian)
{
	int8_t exponent = (int8_t)value_start_little_endian[4];
	uint32_t mantissa = value_start_little_endian[1] +
			(((uint32_t)value_start_little_endian[2]) << 8) +
			(((uint32_t)value_start_little_endian[3]) << 16);

	return (float)mantissa*pow(10,exponent);
}

//client
void gecko_ecen5823_update(struct gecko_cmd_packet *evt)
{
	//thermometer service
	uint8array temp_service;
	temp_service.data[0] = 0x09;
	temp_service.data[1] = 0x18;
	temp_service.len = 2;

	//thermometer characteristic
	uint8array temp_char;
	temp_char.data[0] = 0x1C;
	temp_char.data[1] = 0x2A;
	temp_char.len = 2;


	static uint32 temp_service_handle=0;
	static uint32_t temp_char_handle=0;
	static float temperature;

	//server address and client address
	const uint8_t addr_btserver[] = SERVER_BT_ADDRESS;
	struct gecko_msg_system_get_bt_address_rsp_t *addr_btclient;

	switch (BGLIB_MSG_ID(evt->header))
	{
		case gecko_evt_system_boot_id:
			//print client address
			displayPrintf(DISPLAY_ROW_NAME,"Client");
						addr_btclient = gecko_cmd_system_get_bt_address();
						displayPrintf(DISPLAY_ROW_BTADDR,"%x:%x:%x:%x:%x:%x",addr_btclient->address.addr[5],
								addr_btclient->address.addr[4],addr_btclient->address.addr[3],addr_btclient->address.addr[2],
								addr_btclient->address.addr[1],addr_btclient->address.addr[0]);
						displayPrintf(DISPLAY_ROW_BTADDR2,"%x:%x:%x:%x:%x:%x", addr_btserver[5], addr_btserver[4],
								addr_btserver[3], addr_btserver[2], addr_btserver[1], addr_btserver[0]);
						displayPrintf(DISPLAY_ROW_CONNECTION,"Discovering");
						displayUpdate();


			//Start Discovery
			BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_discovery(le_gap_phy_1m, le_gap_general_discoverable));
		   	break;

		//handling scan response
		case gecko_evt_le_gap_scan_response_id:
			if((addr_btserver[0] == evt->data.evt_le_gap_scan_response.address.addr[0]) &&
					 (addr_btserver[1] == evt->data.evt_le_gap_scan_response.address.addr[1]) &&
					 (addr_btserver[2] == evt->data.evt_le_gap_scan_response.address.addr[2]) &&
					 (addr_btserver[3] == evt->data.evt_le_gap_scan_response.address.addr[3]) &&
					 (addr_btserver[4] == evt->data.evt_le_gap_scan_response.address.addr[4]) &&
					 (addr_btserver[5] == evt->data.evt_le_gap_scan_response.address.addr[5]))
			{
				//Connect after scan response
				BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_connect(evt->data.evt_le_gap_scan_response.address, evt->data.evt_le_gap_scan_response.address_type,le_gap_phy_1m));
			}

			break;

		//handling connection
		case gecko_evt_le_connection_opened_id:
			displayPrintf(DISPLAY_ROW_CONNECTION,"Connected");
			displayUpdate();
			connection_bt_cli = evt->data.evt_le_connection_opened.connection;
			BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_parameters(connection_bt_cli,60,60,3,600));
			//Discover GATT services
			BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_primary_services_by_uuid(evt->data.evt_le_connection_opened.connection, temp_service.len, temp_service.data));
			break;

		//get service handle
		case gecko_evt_gatt_service_id:
			Procedure_complete_evt = service;
			temp_service_handle = evt->data.evt_gatt_service.service;
			break;

		//get characteristic handle
		case gecko_evt_gatt_characteristic_id:
			Procedure_complete_evt = characteristic;
			temp_char_handle = evt->data.evt_gatt_characteristic.characteristic;
			break;

		//handling procedure complete event
		case gecko_evt_gatt_procedure_completed_id:

			if(	Procedure_complete_evt == service)
			{
				//disocver characteristic
				BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_discover_characteristics_by_uuid(evt->data.evt_gatt_procedure_completed.connection,temp_service_handle,temp_char.len,temp_char.data));
				Procedure_complete_evt = NoEvent;

			}

			else if(Procedure_complete_evt == characteristic)
			{
				//set notification
				BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_set_characteristic_notification(evt->data.evt_gatt_procedure_completed.connection,temp_char_handle,gatt_indication));
				Procedure_complete_evt = NoEvent;
			}

			break;

		//get and display temperature characteristic value
		case gecko_evt_gatt_characteristic_value_id:
			if(evt->data.evt_gatt_characteristic_value.characteristic == temp_char_handle)
				{
					BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_send_characteristic_confirmation(connection_bt_cli));
					displayPrintf(DISPLAY_ROW_CONNECTION,"Handling Indications");
					temperature = gattUint32ToFloat(evt->data.evt_gatt_characteristic_value.value.data);
					displayPrintf(DISPLAY_ROW_TEMPVALUE,"Temperature:%.2f",temperature);
					displayUpdate();
				}

			break;

		//restart discovery when connection is closed
		case gecko_evt_le_connection_closed_id:
			BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_discovery(le_gap_phy_1m, le_gap_discover_generic));
			displayPrintf(DISPLAY_ROW_CONNECTION,"Discovering");
			displayPrintf(DISPLAY_ROW_TEMPVALUE," ");
			break;

	}
}


#endif


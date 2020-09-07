#include "ble.h"
#include "ble_device_type.h"

//Run Bluegecko as server
#if DEVICE_IS_BLE_SERVER
void gecko_ecen5823_update(struct gecko_cmd_packet* evt)
{
	 struct gecko_msg_system_get_bt_address_rsp_t *disp_addr;
	 static uint8_t connection_passkey=0;
	 uint8_t button_state = 0;
	 uint8_t pb_buffer[1];
	 uint8_t *p = pb_buffer;
	 uint32_t pb_state_val;

	 switch (BGLIB_MSG_ID(evt->header))
	 {
	      //This boot event is generated when the system boots up after reset.
	      case gecko_evt_system_boot_id:
	    	bond_flag = 0;
	    	displayPrintf(DISPLAY_ROW_NAME,"Server");
	    	disp_addr = gecko_cmd_system_get_bt_address();
	    	displayPrintf(DISPLAY_ROW_BTADDR,"%x:%x:%x:%x:%x:%x",disp_addr->address.addr[5],disp_addr->address.addr[4],disp_addr->address.addr[3],disp_addr->address.addr[2],disp_addr->address.addr[1],disp_addr->address.addr[0]);
	    	displayPrintf(DISPLAY_ROW_CONNECTION,"Advertising");
	    	displayUpdate();
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_delete_bondings());
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_set_bondable_mode(1));
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_configure(MITM_FLAG, sm_io_capability_displayyesno));
    		BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(default_value_adv, interval_min_adv, interval_max_adv, default_value_adv,default_value_adv ));
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
	        GPIO_PinModeSet(gpioPortF, PB_Pin, gpioModeInput, true);
	        GPIO_IntConfig(gpioPortF, PB_Pin, true, true, true);
	        NVIC_EnableIRQ(GPIO_EVEN_IRQn);
	        break;

	     //confirm bonding
	  	case gecko_evt_sm_confirm_bonding_id:
	      	BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_bonding_confirm(evt->data.evt_sm_confirm_bonding.connection,1));
	        break;

	    //Handle event when system is bonded
	    case gecko_evt_sm_bonded_id:
	    	bond_flag = 1;
	      	displayPrintf(DISPLAY_ROW_CONNECTION,"Bonded");
	  		displayPrintf(DISPLAY_ROW_PASSKEY,"");
			displayPrintf(DISPLAY_ROW_ACTION,"");
			displayUpdate();
	      	break;

	    //Hndle event when bonding fails
	    case gecko_evt_sm_bonding_failed_id:
	    	//displayPrintf(DISPLAY_ROW_CONNECTION,"Bonding Failed");
	  		displayPrintf(DISPLAY_ROW_PASSKEY,"");
			displayPrintf(DISPLAY_ROW_ACTION,"");
	  		displayUpdate();
	  		LOG_INFO("Bonding Failed");
	      	break;

	    //Handle event when passkey has been confirmed
	    case gecko_evt_sm_confirm_passkey_id:
	      	connection_passkey = evt->data.evt_sm_confirm_passkey.connection;
	      	displayPrintf(DISPLAY_ROW_PASSKEY,"%lu",evt->data.evt_sm_confirm_passkey.passkey);
	  		displayPrintf(DISPLAY_ROW_ACTION,"Confirm with PB0");
    		displayUpdate();
    		break;

    	//handle event when bluettoth connection is opened
	    case gecko_evt_le_connection_opened_id:
	    	displayPrintf(DISPLAY_ROW_CONNECTION,"Connected");
	    	displayUpdate();
	    	connection_bt=evt->data.evt_le_connection_opened.connection;
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_le_connection_set_parameters(connection_bt,conn_param_min_interval,conn_param_max_interval,latency,timeout));
	    	LOG_INFO("opened");
	    	break;

	    //Handle event when bluettoth connection is closed
	    case gecko_evt_le_connection_closed_id:
	    	displayPrintf(DISPLAY_ROW_CONNECTION,"Advertising");
	        displayUpdate();
	    	displayPrintf(DISPLAY_ROW_TEMPVALUE,"");
	    	displayUpdate();
	    	BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_set_advertise_timing(default_value_adv, interval_min_adv, interval_max_adv, default_value_adv,default_value_adv ));
	        BTSTACK_CHECK_RESPONSE(gecko_cmd_le_gap_start_advertising(0, le_gap_general_discoverable, le_gap_connectable_scannable));
	        gecko_cmd_system_set_tx_power(0);
	    	break;

	    //setting events on external signal
	      case gecko_evt_system_external_signal_id:

	    	  if(evt->data.evt_system_external_signal.extsignals == passkey)
	    	      	{
	    	      		if(bond_flag==1)
	    	     			{
	    	   					button_state = GPIO_PinInGet(gpioPortF, PB_Pin);
	        					button_state = !button_state;
	   	      					LOG_INFO("Button State = %u\n", button_state);
	   	      					pb_state_val = FLT_TO_UINT32(button_state, -3);
	   	      					UINT32_TO_BITSTREAM(p, pb_state_val);
	   	      					BTSTACK_CHECK_RESPONSE(gecko_cmd_gatt_server_send_characteristic_notification(0xFF, gattdb_button_state, 1, pb_buffer));
    	      					displayPrintf(DISPLAY_ROW_CLIENTADDR,"Button State: %u", button_state);
    	      					displayUpdate();
	    	      			}
	    	      			else
	    	      			{
	    	      				BTSTACK_CHECK_RESPONSE(gecko_cmd_sm_passkey_confirm(connection_passkey, true));
	    	      			}
	    	      		}
	    	  else
	    	  {
	    		  //set events for state machine
	    		  setStateMachineEvent(evt);
	    	  }
	    	  break;

	       //set tx power
	       case gecko_evt_le_connection_rssi_id:
	    	   settxpower(evt);
	        	break;

	       default:
	    	   break;
	 }

}

//function to set state machine events
void setStateMachineEvent(struct gecko_cmd_packet* evt)
{
	if(evt->data.evt_system_external_signal.extsignals  == none )
					  {
						  occured_event = none;
					  }
					  else if(evt->data.evt_system_external_signal.extsignals  == period_expired )
					  {
						   occured_event = period_expired;
					  }
					  else if(evt->data.evt_system_external_signal.extsignals == delay )
					  {
							occured_event = delay;
					  }
					  else if(evt->data.evt_system_external_signal.extsignals  == I2CtransferComplete )
					  {
							occured_event = I2CtransferComplete;
					  }
					  else if(evt->data.evt_system_external_signal.extsignals == disableevent )
					  {
							occured_event = disableevent;
					  }
}

//function to set tx power
void settxpower(struct gecko_cmd_packet* evt)
{

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
	}
	else
	{
		tx_power = 80;
	}

	gecko_cmd_system_halt(1);
	gecko_cmd_system_set_tx_power(tx_power);
	gecko_cmd_system_halt(0);
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

			if((evt->data.evt_gatt_service.uuid.data[1] == temp_service.data[1]) && (evt->data.evt_gatt_service.uuid.data[0] == temp_service.data[0]))
			{
				Procedure_complete_evt = service;
				temp_service_handle = evt->data.evt_gatt_service.service;
			}
			break;

		//get characteristic handle
		case gecko_evt_gatt_characteristic_id:
			if((evt->data.evt_gatt_characteristic.uuid.data[1] == temp_char.data[1]) && (evt->data.evt_gatt_characteristic.uuid.data[0] == temp_char.data[0]))
			{
				Procedure_complete_evt = characteristic;
				temp_char_handle = evt->data.evt_gatt_characteristic.characteristic;
			}
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
			displayUpdate();
			break;

	}
}


#endif


/***********************************************************************************

* @ble.h
* @author Ridhi Shah
* This file contains all header files and functions needed by the BLE
* @references: https://docs.silabs.com/bluetooth/latest/

************************************************************************************/
#include "state_handler.h"
#include "gecko_ble_errors.h"
#include "display.h"
#include "gatt_db.h"
#include "native_gecko.h"
#include "infrastructure.h"

//advertising and connection parrameters
#define default_value_adv (0)
#define interval_min_adv (400)
#define interval_max_adv (400)
#define conn_param_min_interval (60)
#define conn_param_max_interval (60)
#define latency (3)
#define timeout (600)

//Configuration Flags
#define MITM_FLAG	(0x01)

//pushbutton gpio pin
#define PB_Pin (6)


//variable to store rssi value;
int32_t rssi_value;

int32_t tx_power;

int bond_flag;

//server and client connections
uint8_t connection_bt;
uint8_t connection_bt_cli;

//enum for client procedure complete events
typedef enum client_bt_event
{
	NoEvent=0,
	service=1,
	characteristic=2,
}client_bt_event;

client_bt_event Procedure_complete_evt;

/*
 * @brief: function to convert uint32 to float
 * @param : value_start_little_endian - value to be converted
 */
float gattUint32ToFloat(const uint8_t *value_start_little_endian);

/*
 * @brief: function to set state machine events
 * @param : evt - bluetoth stack event
 */
void setStateMachineEvent(struct gecko_cmd_packet* evt);

/*
 * @brief: function to set tx power
 * @param : evt - bluetoth stack event
 */
void settxpower(struct gecko_cmd_packet* evt);

/*
 * @brief: function to set events and call state machine according to bluetooth events
 * @param : evt - bluetoth stack event
 */
void gecko_ecen5823_update(struct gecko_cmd_packet* evt);

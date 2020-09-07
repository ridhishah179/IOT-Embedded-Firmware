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

#define TEMP_SERVICE_DATA_0 (0x09)
#define TEMP_SERVICE_DATA_1 (0x18)

#define	TEMP_CHAR_DATA_0   (0x1C)
#define	TEMP_CHAR_DATA_1   (0x2A)

#define PB_SERVICE_VAL_0	(0x89)
#define PB_SERVICE_VAL_1	(0x62)
#define PB_SERVICE_VAL_2	(0x13)
#define PB_SERVICE_VAL_3    (0x2d)
#define PB_SERVICE_VAL_4	(0x2a)
#define PB_SERVICE_VAL_5	(0x65)
#define PB_SERVICE_VAL_6	(0xec)
#define PB_SERVICE_VAL_7	(0x87)
#define PB_SERVICE_VAL_8	(0x3e)
#define PB_SERVICE_VAL_9	(0x43)
#define PB_SERVICE_VAL_10	(0xc8)
#define PB_SERVICE_VAL_11	(0x38)
#define PB_SERVICE_VAL_12	(0x01)
#define PB_SERVICE_VAL_13	(0x00)
#define PB_SERVICE_VAL_14	(0x00)
#define PB_SERVICE_VAL_15	(0x00)

#define PB_CHAR_VAL	(0x02)



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
	pb_char=3,
	pb_service_evt=4

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

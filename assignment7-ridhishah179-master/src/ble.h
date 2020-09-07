#include "state_handler.h"
#include "gecko_ble_errors.h"
#include "display.h"
#include "native_gecko.h"

//variable to store rssi value;
int32_t rssi_value;

int32_t tx_power;

//server and client connections
uint8_t connection_bt;
uint8_t connection_bt_cli;

//enum for client procedure complete events
typedef enum client_bt_event
{
	NoEvent=0,
	service=1,
	characteristic=2

}client_bt_event;

client_bt_event Procedure_complete_evt;

/*
 * @brief: function to convert uint32 to float
 * @param : value_start_little_endian - value to be converted
 */
float gattUint32ToFloat(const uint8_t *value_start_little_endian);


/*
 * @brief: function to set events and call state machine according to bluetooth events
 * @param : evt - bluetoth stack event
 */
void gecko_ecen5823_update(struct gecko_cmd_packet* evt);

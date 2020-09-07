#include "state_handler.h"
#include "gecko_ble_errors.h"
#include "display.h"
#include "native_gecko.h"

//variable to store rssi value;
int32_t rssi_value;

//tx power
int32_t tx_power;

//opened connection
uint8_t connection_bt;

//struct for displaying server addres
struct gecko_msg_system_get_bt_address_rsp_t *disp_addr;

/*
 * @brief: function to set events and call state machine according to bluetooth events
 * @param : evt - bluetoth stack event
 */
void gecko_ecen5823_update(struct gecko_cmd_packet* evt);

/***********************************************************************************

* @i2c.c
* @author Ridhi Shah
* This file contains functions for enabling i2c to measure temperature using Si7021
* @references:https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg1/html/group__I2CSPM.html
* 			  https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg1/html/group__GPIO.html

************************************************************************************/

#include "i2c.h"
#include "letimer.h"
#include "log.h"
#include "cmu.h"
#include "sleep.h"
#include <em_gpio.h>
#include <em_emu.h>
#include "gatt_db.h"
#include "native_gecko.h"
#include "infrastructure.h"
#include "display.h"



uint16_t temp_data = 0;
extern uint8_t connection_bt;



//Initialize I2C SPM structure

void i2c_init()
{
	int i;
	I2C_Init_TypeDef i2cinit = I2C_INIT_DEFAULT;

	//Enable clocks
	CMU_Clock_TypeDef i2cClock;
	CMU_ClockEnable(cmuClock_HFPER, true);
	i2cClock = cmuClock_I2C0;
	CMU_ClockEnable(i2cClock, true);

	//set pin modes for SCL and SDA
	 GPIO_PinModeSet(gpioPortC,10, gpioModeWiredAndPullUp, 1);
	 GPIO_PinModeSet(gpioPortC,11, gpioModeWiredAndPullUp, 1);

	 for (i = 0; i < 9; i++)
	 {
	    GPIO_PinOutSet(gpioPortC, 10);
	    GPIO_PinOutClear(gpioPortC, 11);
	 }

	 //Enable routing
	 I2C0->ROUTELOC0 |= (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SCLLOC_MASK)) | I2C_ROUTELOC0_SCLLOC_LOC14;
	 I2C0->ROUTELOC0 |= (I2C0->ROUTELOC0 & (~_I2C_ROUTELOC0_SDALOC_MASK))| I2C_ROUTELOC0_SDALOC_LOC16;
	 I2C0->ROUTEPEN  = I2C_ROUTEPEN_SDAPEN | I2C_ROUTEPEN_SCLPEN;


	I2C_Init(I2C0,&i2cinit);

	//Enable NVIC interrupt for I2C
	NVIC->ISER[(((uint32_t)I2C0_IRQn) >> 5UL)] |= (uint32_t)(1UL << (((uint32_t)I2C0_IRQn) & 0x1FUL));

	//Enable I2C blovk
	I2C_IntClear(I2C0,I2C_IF_TXC|I2C_IF_START|I2C_IF_ADDR|I2C_IF_RXDATAV|I2C_IF_ACK);
	I2C_Enable(I2C0,1);




}

//Enable sensor pin
void sensor_enable()
{
	GPIO_PinOutSet(gpioPortD,15);//Set sensor Enable pin

}

//Disable sensor and i2c GPIO pins
void sensor_disable()
{

	GPIO_PinOutClear(gpioPortD,15); //clear sensor pin

}

//function to write and read I2C data
void i2c_transfer_write()
{
	uint8_t write_buffer_data[1];

	seq.addr = device_address << 1; //Shift I2C device address by 1 and pass in the Transfer seq structure
	seq.flags = I2C_FLAG_WRITE; //Initialize flags for write sequence
	write_buffer_data[0] = 0xF3; // write command for master no hold mode
	seq.buf[0].data = write_buffer_data; //pass command in transfer seq structure
	seq.buf[0].len = 1; // pass length of write command in transfer seq structure

	i2c_transfer_return_status = I2C_TransferInit(I2C0,&seq); //Transfer the data to be written

	//timerWaitUs(10000);
	//sleep when transfer in progress
	while( i2c_transfer_return_status == i2cTransferInProgress)
	{


		SLEEP_SleepBlockEnd(EM+(1));
		SLEEP_SleepBlockBegin(2);
		//SLEEP_Sleep();
		struct gecko_cmd_packet *evt;
		evt = gecko_wait_event();
		gecko_external_signal(evt->data.evt_system_external_signal.extsignals);
	}


}

void i2c_transfer_read()
{

	seq.addr = device_address << 1;
	seq.flags = I2C_FLAG_READ; //Set flags for read sequence
	seq.buf[0].data =i2c_read_data; //provide buffer for data to be read
	seq.buf[0].len = 2; //set buffer length
	//timerSetEventInMs(5);

	i2c_transfer_return_status = I2C_TransferInit(I2C0,&seq); //transfer data
}

//function to measure temperature when event occurs
void measure_temp()
{

	uint8_t htmTempBuffer[5]; /* Stores the temperature data in the Health Thermometer (HTM) format. */
	uint8_t flags = 0x00;   /* HTM flags set as 0 for Celsius, no time stamp and no temperature type. */
	uint32_t temperature;   /* Stores the temperature data read from the sensor in the correct format */
	uint8_t *p = htmTempBuffer; /* Pointer to HTM temperature buffer needed for converting values to bitstream. */
	float temp_celcius = 0;
	UINT8_TO_BITSTREAM(p, flags);
	temp_data = (uint16_t)(i2c_read_data[0]<<8)|(i2c_read_data[1]);
	temp_celcius = ((175.72*temp_data)/65536)-46.85;
	temperature = FLT_TO_UINT32(temp_celcius*1000, -3);
	  /* Convert temperature to bitstream and place it in the HTM temperature data buffer (htmTempBuffer) */
	UINT32_TO_BITSTREAM(p, temperature);

	gecko_cmd_gatt_server_send_characteristic_notification(connection_bt, gattdb_temperature_measurement, 5, htmTempBuffer);
	displayPrintf(DISPLAY_ROW_TEMPVALUE,"Temperature: %.2f",temp_celcius);
	displayUpdate();
	LOG_INFO("Temp - %f",temp_celcius);
	gecko_cmd_le_connection_get_rssi(connection_bt);

}

//Interrupt handle for I2C
void I2C0_IRQHandler(void)
{
	CORE_ATOMIC_IRQ_DISABLE();
	i2c_transfer_return_status = I2C_Transfer(I2C0); //transfer values to read and write when inteerupt occurs
	if( i2c_transfer_return_status != i2cTransferInProgress)
	{
		gecko_external_signal(I2CtransferComplete); //if transfer is complete set event
	}
	CORE_ATOMIC_IRQ_ENABLE();
 }

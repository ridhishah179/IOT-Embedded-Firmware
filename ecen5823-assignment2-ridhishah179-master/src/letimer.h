/***********************************************************************************

* @lettimer.h
* @author Ridhi Shah
* This file contains header files for configuring LETIMER0 and Macros to specify period and ON time of LED
* @references: https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg13/html/group__LETIMER.html

************************************************************************************/

#include <em_letimer.h>
#include <em_core.h>
#include <math.h>


#define PERIOD (2250) //Configure the desired period for the LED
#define ON (175) //Configure LED ON time

/*
 * @brief: This function calculates and sets the values of the compare register according to the
 *        specified period and on time
 */
void set_compare_regs();


/*
 * @brief: This function configures and enables the LETIMR0 for blinking LED accorfing to the specified period and on Time
 */
void letimer_setup();

/*
 * @brief: This function handles the COMP0 and underflow interrupts and toggles LED accordingly
 */
void LETIMER0_IRQHandler(void);


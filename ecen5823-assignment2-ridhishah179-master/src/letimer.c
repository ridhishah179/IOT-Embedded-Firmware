/***********************************************************************************

* @lettimer.c
* @author Ridhi Shah
* This file contains functions to configure LETIMER0 for blinking an LED according
  to a specified period and Energy mode
* @references: https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg13/html/group__LETIMER.html
			   https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg1/html/group__CORE.html

************************************************************************************/

#include "letimer.h"
#include "cmu.h"

//Configure LETIMER0
void letimer_setup()
{
	LETIMER_Init_TypeDef conf_letimer = LETIMER_INIT_DEFAULT;
	conf_letimer.comp0Top = true;

	LETIMER_Init(LETIMER0, &conf_letimer); //Initialize LETIMER0
	LETIMER_IntEnable(LETIMER0,LETIMER_IEN_UF | LETIMER_IEN_COMP1); //Enable Underflow and COMP1 interrupts
	NVIC_EnableIRQ(LETIMER0_IRQn); //Enable NVIC IRQ
	set_compare_regs(); // Call function to set compare register values
	LETIMER_IntClear(LETIMER0,LETIMER_IFC_COMP1|LETIMER_IFC_UF); // Clear the interrupt flags
	LETIMER_Enable(LETIMER0, true); // Enable LETIMER0


}

//Calculate and set Compare register values
void set_compare_regs()
{
	int comp0_val;
	int comp1_val;
	float period = PERIOD/1000.0;
	float on = ON/1000.0;

//Calculate COMP0 and COMP1 values for modes lower than EM3
if (EM<3)
	{
		int freq = 32768;
		int power = 0;

		//Calculate Clock Prescaler
		if(freq*period>65535)
		{
			while(freq*period > 65535)
			{
				freq=freq/2;
				power++;
			}

			CMU_ClockDivSet(cmuClock_LETIMER0,pow(2,power)); //Set Clock prescaler

		}

		comp0_val = freq*period;
		comp1_val = ((period*freq)-(freq * on));

	}

// Calculate COMP0 and COMP1 values for EM3
else
	{
		int freq = 1000;
		comp0_val = (freq*period);
		comp1_val = (freq*period)-(freq * on);

	}

	//Set COMP0 and COMP1 values
	LETIMER_CompareSet(LETIMER0,0,comp0_val);
	LETIMER_CompareSet(LETIMER0,1,comp1_val);

}

//Handle COMP1 and UNDERFLOW interrupts to blink LED accordingly
void LETIMER0_IRQHandler(void)
{
	CORE_ATOMIC_IRQ_DISABLE(); //Disable Global interrupts

	int letimer_flags = LETIMER_IntGet(LETIMER0); //Read Interrupt flag register

	//Switch off the LED on COMP1 interrupt
	if(letimer_flags & LETIMER_IF_COMP1)
	{
		GPIO_PinOutClear(gpioPortF, 4);
		LETIMER_IntClear(LETIMER0,LETIMER_IFC_COMP1);
	}

	//Switch on LED on underflow interrupt
	else if(letimer_flags & LETIMER_IF_UF)
	{
		GPIO_PinOutSet(gpioPortF, 4);
		LETIMER_IntClear(LETIMER0,LETIMER_IFC_UF);
	}

	CORE_ATOMIC_IRQ_ENABLE(); //Enable global interrupts
}

/***********************************************************************************

* @main.c
* @author Ridhi Shah
* @references: https://siliconlabs.github.io/Gecko_SDK_Doc/efr32bg1/html/group__SLEEP.html

************************************************************************************/
#include "gecko_configuration.h"
#include "gpio.h"
#include "native_gecko.h"
#include "letimer.h"
#include "cmu.h"
#include "em_emu.h"
#include <sleep.h>


//const SLEEP_EnergyMode_t sleep_mode_blocked=sleepEM3;//configure mode to be blocked

int appMain(gecko_configuration_t *config)
{

  //Initialize stack
  gecko_init(config);
  gpioInit();
  if(EM==1 || EM==2)
  {
	  int block_mode = (EM + (1));
	  SLEEP_SleepBlockBegin(block_mode);
  }

  cmu_setup();
  letimer_setup();

  /* Infinite loop */
  while (1)
  {
	 if(EM==1 || EM==2)
	 {
		SLEEP_Sleep();
	 }
	 else if(EM==3)
	 {
		 EMU_EnterEM3(true);
	 }

  }
}

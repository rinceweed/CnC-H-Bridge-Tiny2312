/*===============================================================================================================================

  Revision     : $Revision: 1.5 $
  Date         : $Date: 2009/08/25 20:02:35 $

  Abstract     : Provides methods to manipulate pages.

 ================================================================================================================================*/
/*--[ Std Include Files ]------------------------------------------------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <avr/interrupt.h>

/*--[ Include Files ]------------------------------------------------------------------------------------------------------------*/
#include "TDefinitions.h"
#include "./bord/Bord.h"
#include "./stepper/stepper.h"
/*--[ Literals ]-----------------------------------------------------------------------------------------------------------------*/

/*--[ Types ]--------------------------------------------------------------------------------------------------------------------*/

/*--[ Constants ]----------------------------------------------------------------------------------------------------------------*/

/*--[ Data ]---------------------------------------------------------------------------------------------------------------------*/

/*--[ Prototypes ]---------------------------------------------------------------------------------------------------------------*/

/*==[ PUBLIC FUNCTIONS ]=========================================================================================================*/

/*==[ PRIVATE FUNCTIONS ]========================================================================================================*/

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
/*!

  Main program. Init die bordjie dan al die peripheals, en voer die call-back event uit die hele tyd

*/
/*-------------------------------------------------------------------------------------------------------------------------------*/
int main(void)
{
  cli();
  Bord_Init();
  /*Has to be the first APP*/
  Stepper_Initialisation();

  while(1)
  {
  }

  return(0);
}

/*EOF============================================================================================================================*/

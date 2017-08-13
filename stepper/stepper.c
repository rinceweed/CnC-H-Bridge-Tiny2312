 /*-------------------------------------------------------------------------------------------------------------------------------*/
/*--[ Visibility ]---------------------------------------------------------------------------------------------------------------*/
/*--[ Include Files ]------------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
//#include <math.h>
#include <avr/io.h>
//#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <compat/deprecated.h>
//#include <util/delay.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../TDefinitions.h"
#include "../bord/Bord.h"
#include "../hbridge/HBridgeBiPolar.h"
#include "stepper.h"

/*--[ Literals ]-----------------------------------------------------------------------------------------------------------------*/

#define DIR_FWD                   (0x0)
#define DIR_REV                   (0x1)
#define MOVE_STEP                 (0x1)
#define MOVE_IDLE                 (0x0)

#define  Int0mask (1 << INT0)
#define  Isc0mask (1 << ISC00) //Any edge
#define  Int1mask (1 << INT1)
#define  Isc1mask (1 << ISC10) //Any edge

/*--[ Types ]--------------------------------------------------------------------------------------------------------------------*/

/*--[ Constants ]----------------------------------------------------------------------------------------------------------------*/

/*--[ Data ]---------------------------------------------------------------------------------------------------------------------*/

/*--[ Prototypes ]---------------------------------------------------------------------------------------------------------------*/
static void step(enum PortLowHigh H, uint8_t step, uint8_t dir);

/*==[ PUBLIC FUNCTIONS ]=========================================================================================================*/
/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
void Stepper_Initialisation(void)
{
  HBridgeBiPolar_new();
  cli();
  GIMSK = (Int0mask|Int1mask);    // Enable INTx
  MCUCR = (Isc0mask|Isc1mask); //Trigger on any edge
  //Clear any pending interrupts, only start using from here on really
  EIFR  = ((1<<INTF0)|(1<<INTF1));
  sei();        //Enable Global Interrupt
}

/*==[ INTERRUPT FUNCTIONS ]===========================================================================================================*/
//Take any edge trigger and step or not
SIGNAL(INT0_vect)
{
  step(PortHIGH, STEP_X, DIR_X);
}

//Take any edge trigger and step or not
SIGNAL(INT1_vect)
{
  step(PortLOW, STEP_X, DIR_X);
}
/*==[ PRIVATE FUNCTIONS ]========================================================================================================*/

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
//On each timer tick each stepper is evaluated if it need to step in a direction
//When none is stepped it means the step sequence is completed, the timer is stopped and the end is indicated to the
//cyclic handler to provide new steps (if there is something in the q)
static void step(enum PortLowHigh H, uint8_t step, uint8_t dir)
{
  if (((STEP_PIN >> step) & 0x01) == MOVE_STEP)
  {
    if (((STEP_PIN >> dir) & 0x01) == DIR_FWD)
    {
      HBridgeBiPolar_step_forward(H);
    }
    else
    {
      HBridgeBiPolar_step_backward(H);
    }
  }
  else
  {
    HBridgeBiPolar_stop(H);;
  }
  return;
}

/*EOF============================================================================================================================*/

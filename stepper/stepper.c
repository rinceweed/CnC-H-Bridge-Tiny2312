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

/*--[ Literals ]-----------------------------------------------------------------------------------------------------------------*/
#define NUM_HBRIDGES_BIPOLAR            (2)

/*--[ Types ]--------------------------------------------------------------------------------------------------------------------*/
enum PortLowHigh
{
	PortLOW = 0,
	PortHIGH,
	MAX_PortLowHigh
}; 

/*--[ Constants ]----------------------------------------------------------------------------------------------------------------*/

/*--[ Data ]---------------------------------------------------------------------------------------------------------------------*/

/*--[ Prototypes ]---------------------------------------------------------------------------------------------------------------*/
//void step(enum PortLowHigh H, uint8_t step, uint8_t dir);


/*--[ Literals ]-----------------------------------------------------------------------------------------------------------------*/
#define MAX_StepSIGNLE                    (4)

/*--[ Constants ]----------------------------------------------------------------------------------------------------------------*/
uint8_t NormalStepSteps[MAX_PortLowHigh][MAX_StepSIGNLE] PROGMEM  =
{
  { 0x0A, 0x06, 0x05, 0x09 },
  { 0xA0, 0x60, 0x50, 0x90 }
};

uint8_t HiLoMask[MAX_PortLowHigh] PROGMEM  =
{
  0xF0, 0x0F
};

uint8_t HiLoEnable[MAX_PortLowHigh] PROGMEM  =
{
  CONTROL_OUT_HBRIDGE_ENABLE_L, CONTROL_OUT_HBRIDGE_ENABLE_H
};
/*--[ Prototypes ]---------------------------------------------------------------------------------------------------------------*/

static void step(enum PortLowHigh port, uint8_t L_H);

/*--[ Data ]---------------------------------------------------------------------------------------------------------------------*/
volatile int8_t Hbridge_StepIdx[MAX_PortLowHigh];
volatile uint8_t Hbridge_Port_Value;



/*==[ PUBLIC FUNCTIONS ]=========================================================================================================*/
/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
void Stepper_Initialisation(void)
{
  sbi(CONTROL_PORT, CONTROL_OUT_LED);

  cli();
  Hbridge_StepIdx[PortLOW] = 0;
  Hbridge_StepIdx[PortHIGH] = 0;
  Hbridge_Port_Value = (pgm_read_byte(&(NormalStepSteps[PortLOW][0])) | pgm_read_byte(&(NormalStepSteps[PortHIGH][0])));
  HBRIDGE_PORT = Hbridge_Port_Value;

  MCUCR = (PUD|Isc0mask|Isc1mask);    //Trigger on any edge
  GIMSK = (Int0mask|Int1mask);    // Enable INTx

  //Clear any pending interrupts, only start using from here on really
  EIFR  = ((1<<INTF0)|(1<<INTF1));
  sei();        //Enable Global Interrupt

  cbi(CONTROL_PORT, CONTROL_OUT_LED);
}

/*==[ INTERRUPT FUNCTIONS ]===========================================================================================================*/
//Take any edge trigger and step or not
ISR(INT0_vect)
{
  step(PortHIGH, CONTROL_IN_STEP_H);
}

//Take any edge trigger and step or not
ISR(INT1_vect)
{
  step(PortLOW, CONTROL_IN_STEP_L);
}
/*==[ PRIVATE FUNCTIONS ]========================================================================================================*/

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
static void step(enum PortLowHigh port, uint8_t L_H)
{
  uint8_t control = CONTROL_PIN;
  if (((control >> L_H) & 0x01) == MOVE_STEP)
  {
    if (((control >> L_H) & 0x01) == DIR_FWD)
    {
      (Hbridge_StepIdx[port])++;
      if (Hbridge_StepIdx[port] >= MAX_StepSIGNLE)
      {
       Hbridge_StepIdx[port] = 0;
      }
    }
    else
    {
      (Hbridge_StepIdx[port])--;
      if (Hbridge_StepIdx[port] < 0)
      {
        Hbridge_StepIdx[port] = (MAX_StepSIGNLE - 1);
      }
    }

    uint8_t mask = pgm_read_byte(&(HiLoMask[port]));
    uint8_t hbridge_port = (Hbridge_Port_Value & mask);
    uint8_t step_idx = Hbridge_StepIdx[port];
    Hbridge_Port_Value = (hbridge_port | pgm_read_byte(&(NormalStepSteps[port][step_idx])));
    HBRIDGE_PORT = Hbridge_Port_Value;

    sbi(CONTROL_PORT, pgm_read_byte(&(HiLoEnable[port])));
  }
  else
  {
    cbi(CONTROL_PORT, pgm_read_byte(&(HiLoEnable[port])));
  }
  return;
}

/*EOF============================================================================================================================*/

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
void step(enum PortLowHigh H, uint8_t step, uint8_t dir);


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
static void stepH(enum PortLowHigh H);

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

  MCUCR = (Isc0mask|Isc1mask);    //Trigger on any edge
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
  uint8_t control = CONTROL_PIN;
  if (((control >> CONTROL_IN_STEP_H) & 0x01) == MOVE_STEP)
  {
    if (((control >> CONTROL_IN_STEP_DIR_H) & 0x01) == DIR_FWD)
    {
      (Hbridge_StepIdx[PortHIGH])++;
      if (Hbridge_StepIdx[PortHIGH] >= MAX_StepSIGNLE)
      {
       Hbridge_StepIdx[PortHIGH] = 0;
      }
    }
    else
    {
      (Hbridge_StepIdx[PortHIGH])--;
      if (Hbridge_StepIdx[PortHIGH] < 0)
      {
        Hbridge_StepIdx[PortHIGH] = (MAX_StepSIGNLE - 1);
      }
    }

    uint8_t mask = pgm_read_byte(&(HiLoMask[PortHIGH]));
    uint8_t hbridge_port = (Hbridge_Port_Value & mask);
    uint8_t step_idx = Hbridge_StepIdx[PortHIGH];
    Hbridge_Port_Value = (hbridge_port | pgm_read_byte(&(NormalStepSteps[PortHIGH][step_idx])));
    HBRIDGE_PORT = Hbridge_Port_Value;

    sbi(CONTROL_PORT, HiLoEnable[PortHIGH]); 
  }
  else
  {
    cbi(CONTROL_PORT, HiLoEnable[PortHIGH]); 
  }
}

//Take any edge trigger and step or not
ISR(INT1_vect)
{
  uint8_t control = CONTROL_PIN;
  if (((control >> CONTROL_IN_STEP_L) & 0x01) == MOVE_STEP)
  {
    if (((control >> CONTROL_IN_STEP_DIR_L) & 0x01) == DIR_FWD)
    {
      (Hbridge_StepIdx[PortLOW])++;
      if (Hbridge_StepIdx[PortLOW] >= MAX_StepSIGNLE)
      {
       Hbridge_StepIdx[PortLOW] = 0;
      }
    }
    else
    {
      (Hbridge_StepIdx[PortLOW])--;
      if (Hbridge_StepIdx[PortLOW] < 0)
      {
        Hbridge_StepIdx[PortLOW] = (MAX_StepSIGNLE - 1);
      }
    }

    uint8_t mask = pgm_read_byte(&(HiLoMask[PortLOW]));
    uint8_t hbridge_port = (Hbridge_Port_Value & mask);
    uint8_t step_idx = Hbridge_StepIdx[PortLOW];
    Hbridge_Port_Value = (hbridge_port | pgm_read_byte(&(NormalStepSteps[PortLOW][step_idx])));
    HBRIDGE_PORT = Hbridge_Port_Value;

    sbi(CONTROL_PORT, HiLoEnable[PortLOW]); 
  }
  else
  {
    cbi(CONTROL_PORT, HiLoEnable[PortLOW]); 
  }
}
/*==[ PRIVATE FUNCTIONS ]========================================================================================================*/

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
//On each timer tick each stepper is evaluated if it need to step in a direction
//When none is stepped it means the step sequence is completed, the timer is stopped and the end is indicated to the
//cyclic handler to provide new steps (if there is something in the q)
void step(enum PortLowHigh H, uint8_t step, uint8_t dir)
{
/*  uint8_t control = CONTROL_PIN;
  if (((control >> step) & 0x01) == MOVE_STEP)
  {
    if (((control >> dir) & 0x01) == DIR_FWD)
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
    HBridgeBiPolar_stop(H);
  }
  */
  return;
}

/*EOF============================================================================================================================*/

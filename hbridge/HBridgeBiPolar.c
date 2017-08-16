/*-------------------------------------------------------------------------------------------------------------------------------*/

/*--[ Visibility ]---------------------------------------------------------------------------------------------------------------*/

/*--[ Include Files ]------------------------------------------------------------------------------------------------------------*/
//#include <stdint.h>
//#include <stddef.h>
//#include <stdbool.h>
//#include <math.h>
#include <avr/io.h>
#include <compat/deprecated.h>
//#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../TDefinitions.h"
#include "../bord/Bord.h"
#include "HBridgeBiPolar.h"

/*--[ Literals ]-----------------------------------------------------------------------------------------------------------------*/
#define MAX_StepSIGNLE                    (4)

/*--[ Types ]--------------------------------------------------------------------------------------------------------------------*/
/*Port with reference to a table step*/

/*PA3 = Pin1 = ~B, PA2 = Pin3 = B, PA1 = Pin1 = ~A, PA0 = Pin3 = A */
/*PC7 = Pin1 = ~B, PC6 = Pin3 = B, PC5 = Pin1 = ~A, PC4 = Pin3 = A */
/*Port with reference to a table step*/

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
uint8_t Hbridge_StepIdx[MAX_PortLowHigh]={0,0};
uint8_t Hbridge_Port_Value =0;
 
/*==[ PUBLIC FUNCTIONS ]=========================================================================================================*/

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
void HBridgeBiPolar_new()
{
  Hbridge_StepIdx[PortLOW] = 0;
  Hbridge_StepIdx[PortHIGH] = 0;
  stepH(PortLOW);
  stepH(PortHIGH);
  return;
}

/*==[ PUBLIC FUNCTIONS ]=========================================================================================================*/
/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
void HBridgeBiPolar_step_forward(enum PortLowHigh H)
{
  (Hbridge_StepIdx[H])++;
  if (Hbridge_StepIdx[H] >= MAX_StepSIGNLE)
  {
   Hbridge_StepIdx[H] = 0;
  }

  uint8_t mask = pgm_read_byte(&(HiLoMask[H]));
  uint8_t hbridge_port = (Hbridge_Port_Value & mask);
  uint8_t step_idx = Hbridge_StepIdx[H];
  Hbridge_Port_Value = (hbridge_port | pgm_read_byte(&(NormalStepSteps[H][step_idx])));
  HBRIDGE_PORT = Hbridge_Port_Value;

  sbi(CONTROL_PORT, HiLoEnable[H]); 
}

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
void HBridgeBiPolar_step_backward(enum PortLowHigh H)
{
  (Hbridge_StepIdx[H])--;
  if (Hbridge_StepIdx[H] < 0)
  {
    Hbridge_StepIdx[H]= (MAX_StepSIGNLE - 1);
  }
  uint8_t mask = pgm_read_byte(&(HiLoMask[H]));
  uint8_t hbridge_port = (Hbridge_Port_Value & mask);
  uint8_t step_idx = Hbridge_StepIdx[H];
  Hbridge_Port_Value = (hbridge_port | pgm_read_byte(&(NormalStepSteps[H][step_idx])));
  HBRIDGE_PORT = Hbridge_Port_Value;

  sbi(CONTROL_PORT, HiLoEnable[H]); 
}

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
void HBridgeBiPolar_stop(enum PortLowHigh H)
{
  cbi(CONTROL_PORT, HiLoEnable[H]); 
}

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
static void stepH(enum PortLowHigh H)
{
}
/*EOF============================================================================================================================*/

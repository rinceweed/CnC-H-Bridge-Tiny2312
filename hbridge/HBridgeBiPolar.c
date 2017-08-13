/*-------------------------------------------------------------------------------------------------------------------------------*/

/*--[ Visibility ]---------------------------------------------------------------------------------------------------------------*/

/*--[ Include Files ]------------------------------------------------------------------------------------------------------------*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
//#include <math.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <compat/deprecated.h>
#include <util/delay.h>
#include <stdlib.h>
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
const uint8_t NormalStepSteps[MAX_PortLowHigh][MAX_StepSIGNLE]  =
{
  { 0x0A, 0x06, 0x05, 0x09 },
  { 0xA0, 0x60, 0x50, 0x90 }
};

const uint8_t HiLoMask[MAX_PortLowHigh]  =
{
    0xF0, 0x0F
};

/*--[ Prototypes ]---------------------------------------------------------------------------------------------------------------*/

/*--[ Data ]---------------------------------------------------------------------------------------------------------------------*/
static uint8_t Hbridge_StepIdx[NUM_HBRIDGES_BIPOLAR];
 
/*==[ PUBLIC FUNCTIONS ]=========================================================================================================*/

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
void HBridgeBiPolar_new()
{
  Hbridge_StepIdx[0] = 0;
  Hbridge_StepIdx[1] = 0;
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
  HBRIDGE_PORT = ((HBRIDGE_PORT & HiLoMask[H]) | NormalStepSteps[H][Hbridge_StepIdx[H]]);
}

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
void HBridgeBiPolar_step_backward(enum PortLowHigh H)
{
  (Hbridge_StepIdx[H])--;
  if (Hbridge_StepIdx[H] < 0)
  {
    Hbridge_StepIdx[H]= (MAX_StepSIGNLE- 1);
  }
  HBRIDGE_PORT = ((HBRIDGE_PORT & HiLoMask[H]) | NormalStepSteps[H][Hbridge_StepIdx[H]]);
}

/*--[ Function ]-----------------------------------------------------------------------------------------------------------------*/
void HBridgeBiPolar_stop(enum PortLowHigh H)
{
  HBRIDGE_PORT = (HBRIDGE_PORT & HiLoMask[H]); 
}

/*EOF============================================================================================================================*/

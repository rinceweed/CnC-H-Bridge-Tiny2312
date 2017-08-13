/*===============================================================================================================================

  Revision     : $Revision: 1.1 $
  Date         : $Date: 2009/08/17 08:11:55 $

  Abstract     : Provides methods to Drive the drivechain.

 ================================================================================================================================*/

#ifndef HBRIDGE_BiPolar_H_
#define HBRIDGE_BiPolar_H_
/*--[ Include Files ]------------------------------------------------------------------------------------------------------------*/

/*--[ Literals ]-----------------------------------------------------------------------------------------------------------------*/
#define NUM_HBRIDGES_BIPOLAR            (2)

/*--[ Types ]--------------------------------------------------------------------------------------------------------------------*/
enum PortLowHigh
{
	PortLOW = 0,
	PortHIGH,
	MAX_PortLowHigh
};

/*--[ Prototypes ]---------------------------------------------------------------------------------------------------------------*/
void HBridgeBiPolar_new();
void HBridgeBiPolar_step_forward(enum PortLowHigh H);
void HBridgeBiPolar_step_backward(enum PortLowHigh H);
void HBridgeBiPolar_stop(enum PortLowHigh H);

/*EOF============================================================================================================================*/
#endif

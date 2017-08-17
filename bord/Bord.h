/*===============================================================================================================================

  Revision     : $Revision: 1.3 $
  Date         : $Date: 2009/08/29 16:24:54 $

  Abstract     : Provides methods to manipulate pages.

 ================================================================================================================================*/

#ifndef BORD_H_
#define BORD_H_
/*--[ Include Files ]------------------------------------------------------------------------------------------------------------*/

/*--[ Literals ]-----------------------------------------------------------------------------------------------------------------*/
/* System clock 4(MHz) in Hz. */
//#define SYSCLK                        (4000000UL)
//S#define PRE_LOAD_TIM0                 (0x64) /* 156 = 10ms :.255-156 = 99 */

#define HBRIDGE_DDR                    DDRB
#define HBRIDGE_PORT                   PORTB
#define HBRIDGE_PIN                    PINB

#define CONTROL_DDR                    DDRD
#define CONTROL_PORT                   PORTD
#define CONTROL_PIN                    PIND

#define CONTROL_OUT_HBRIDGE_ENABLE_H   PD0
#define CONTROL_OUT_HBRIDGE_ENABLE_L   PD1
#define CONTROL_IN_STEP_H              PD2
#define CONTROL_IN_STEP_L              PD3
#define CONTROL_IN_STEP_DIR_H          PD4
#define CONTROL_IN_STEP_DIR_L          PD5

#define CONTROL_OUT_LED                PD6
#define CONTROL_OUT_MASK               ((1<<CONTROL_OUT_HBRIDGE_ENABLE_H)|(1<<CONTROL_OUT_HBRIDGE_ENABLE_L)|(1<<CONTROL_OUT_LED))
#define CONTROL_IN_MASK                ((1<<CONTROL_IN_STEP_H)|(1<<CONTROL_IN_STEP_L)|(1<<CONTROL_IN_STEP_DIR_H)|(1<<CONTROL_IN_STEP_DIR_L))

/*--[ Types ]--------------------------------------------------------------------------------------------------------------------*/

/*--[ Prototypes ]---------------------------------------------------------------------------------------------------------------*/
void Bord_Init(void);

/*EOF============================================================================================================================*/
#endif

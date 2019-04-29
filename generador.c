/*
 * generador.c
 *
 *  Created on: 28/04/2019
 *      Authors: Ruben and Diego Charles
 */
#include "generador.h"
/** defines for signal generator luts*/
#define PULSE_WIDTH_SQUARE_WAVE 	2000u
#define TRIANGULAR_HALF_VALUE 		80u
#define TRIANGULAR_END_VALUE 		160u
#define SINUSOIDAL_LUT_VALUES 		161u
/** defines fot state machine of signal generator*/
#define NUMBER_OF_STATES 			4u


static const uint16_t lut_for_triangular[SINUSOIDAL_LUT_VALUES] =
{
		   0,  50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950,1000,1050,1100,1150,
		1200,1250,1300,1350,1400,1450,1500,1550,1600,1650,1700,1750,1800,1850,1900,1950,2000,2050,2100,2150,2200,2250,2300,2350,
		2400,2450,2500,2550,2600,2650,2700,2750,2800,2850,2900,2950,3000,3050,3100,3150,3200,3250,3300,3350,3400,3450,3500,3550,
		3600,3650,3700,3750,3800,3850,3900,3950,4000,3950,3900,3850,3800,3750,3700,3650,3600,3550,3500,3450,3400,3350,3300,3250,
		3200,3150,3100,3050,3000,2950,2900,2850,2800,2750,2700,2650,2600,2550,2500,2450,2400,2350,2300,2250,2200,2150,2100,2050,
		2000,1950,1900,1850,1800,1750,1700,1650,1600,1550,1500,1450,1400,1350,1300,1250,1200,1150,1100,1050,1000, 950, 900, 850,
		 800, 750, 700, 650, 600, 550,500, 450,  400, 350, 300, 250, 200, 150, 100,  50,  0
};
static const uint16_t lut_for_sine[SINUSOIDAL_LUT_VALUES] =
{
	2000, 2079, 2157, 2235, 2313, 2390,	2467, 2543, 2618, 2692,	2765, 2837,	2908, 2977,	3045, 3111,
	3176, 3238,	3299, 3358,	3414, 3469,	3521, 3571,	3618, 3663,	3705, 3745,	3782, 3816,	3848, 3876,
	3902, 3925,	3945, 3962,	3975, 3986,	3994, 3998,	4000, 3998,	3994, 3986,	3975, 3962,	3945, 3925,
	3902, 3876,	3848, 3816,	3782, 3745,	3705, 3663,	3618, 3571,	3521, 3469,	3414, 3358,	3299, 3238,
	3176, 3111,	3045, 2977,	2908, 2837,	2765, 2692,	2618, 2543,	2467, 2390,	2313, 2235,	2157, 2079,
	2000, 1921,	1843, 1765,	1687, 1610,	1533, 1457,	1382, 1308,	1235, 1163,	1092, 1023,	 955,  889,
	 824,  762,	 701,  642,	 586,  531,	 479,  429,	 382,  337,	 295,  255,  218,  184,	 152,  124,
	  98,  	75,	  55, 	38,   25,	14,	   6,	 2,	   0,    2,	   6,	14,   25,   38,	  55, 	75,
	  98,  124,  152,  184,  218,  255,  295,  337,	 382,  429,  479,  531,  586,  642,  701,  762,
	 824,  889,  955, 1023, 1092, 1163, 1235, 1308,	1382, 1457, 1533, 1610,	1687, 1765, 1843, 1921,
	2000
};
/**  now defining the machine state for signal generator*/
typedef enum{
		IDLE,
		CUADRADA,
		SENOIDAL,
		TRIANGULAR

}state_name_t;/** the machine is composed by four states or 3 signals and idle*/


typedef struct
{
	void (* fptr_signal_to_gen)(void);/* to call  specific generator function*/
}state_t;/** definition of a state of the state machine*/

/** definition of the state machine called gen_FSM*/
const state_t gen_FSM [NUMBER_OF_STATES]=
{
	{gen_idle           },
	{generador_cuadrada},
	{generador_senoidal},
	{generador_triangular}
};
void init_generador_seniales()
{
	//first initializing  the sw3 with her IRQ
	init_sw3(PRIORITY_10,PRIORITY_4,INTR_FALLING_EDGE);
	/** initialize pit0*/
	PIT_clock_gating();
	PIT_enable();
	PIT_delay( SELECTED_PIT , F_CPU, PIT_DELAY);
	//habilitando las interrupciones para el pit_0
	NVIC_enable_interrupt_and_priotity(PIT_CH0_IRQ, PRIORITY_5);
	NVIC_global_enable_interrupts;//equal to macro sei() in AVRS :)
}
void generador_seniales()
{
	static uint8_t current_state=IDLE;		/*the machine begins on idle*/
	static uint8_t sw3_counter=0;     		/* the sw3 haven´t been pressed on this initialization*/
	/* if the sw3 is pressed a corresponding flag interrupt will be set*/
	if (1 == GPIO_get_irq_status(GPIO_A))
	{
		sw3_counter++;						/* every press will increment the state of the machine*/
		GPIO_clear_irq_status(GPIO_A);		/*flag must be cleared, if it not the state machine will change when it shouldn't*/
	}
	/*very important to check overflow*/
	if( NUMBER_OF_STATES <=sw3_counter)
	{
		sw3_counter = 0;
	}
	current_state=sw3_counter;		 	        	/*here the current state of machine changes*/
	gen_FSM[current_state].fptr_signal_to_gen();	/*generating current state signal from current state index*/
}
void gen_idle()
{
 uint8_t DMA_source_addr =0;
}

void generador_cuadrada()
{
	 uint8_t DMA_source_addr =0;
}

void generador_senoidal()
{
	 uint8_t DMA_source_addr =0;
}

void generador_triangular()
{
	 uint8_t DMA_source_addr =0;
}

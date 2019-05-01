/*
 * generador.h
 *
 *  Created on: 28/04/2019
 *      Authors: Ruben y diego charles
 */

#ifndef GENERADOR_H_
#define GENERADOR_H_

#include "MK64F12.h"
#include "NVIC.h"
#include "GPIO.h"
#include "PIT.h"

#include "generador.h"
#include "GPIO.h"
#include "switches_k64.h"

#define F_CPU     		(21000000U)
#define PIT_DELAY 		(0.000006F)
#define SELECTED_PIT 	PIT_0
#define NUMBER_OF_STATES 			3u

/*empty state, it does nothing*/
void gen_idle();
/*generates with the DAC a 5 5kHz square signal*/
void generador_cuadrada();

/* generates with the DAC a 5 kHz sinusoidal signal*/
void generador_senoidal();

/*geneates with the DAC a 5 kHz triangular signal */
void generador_triangular();

/*generates status leds for generador FSM*/
void generador_led(uint8_t l1_state,uint8_t l2_state);

/* init the SW3 with interrup, PIT1 and pins for external leds for generador LED function*/
void init_generador_seniales();

/*general process of generador de señales FSM*/
void generador_seniales();


#endif /* GENERADOR_H_ */

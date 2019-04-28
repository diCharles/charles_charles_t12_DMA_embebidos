/*
 * switches_k64.h
 *
 *  Created on: 09/03/2019
 *      Author: Charles
 */

#ifndef SWITCHES_K64_H_
#define SWITCHES_K64_H_

#include "NVIC.h"
#include "GPIO.h"
#include "Bits.h"

/* DEFINES TO BE USED IN ENABLE SW2 and enable sw3*/
#define  NO_PRIORITY_TH      0
#define  NO_PRIORITY         0
#define  NO_INT_EDGE_DEFINED 0

/* priority threshold levels for interrupts contained on NVIC.h

 		enum type that defines the priority levels for the NVIC.
		 The highest priority is PRIORITY_0 and the lowest PRIORITY_15

		    typedef enum {PRIORITY_0, PRIORITY_1, PRIORITY_2, PRIORITY_3, PRIORITY_4, PRIORITY_5, PRIORITY_6,
					  PRIORITY_7, PRIORITY_8, PRIORITY_9, PRIORITY_10, PRIORITY_11, PRIORITY_12, PRIORITY_13,
					  PRIORITY_14, PRIORITY_15 } priority_level_t;

 */


/* defines for interrupt edge fow argument interruptEdge of fuctions for init_sw2 and init_sw3 as seen on GPIO.h
 Sets Interrupt when logic 0
 	 #define INTR_LOGIC0        0x00080000
 Sets Interrupt on rising-edge.
	#define INTR_RISING_EDGE   0x00090000
Sets Interrupt on falling-edge.
		#define INTR_FALLING_EDGE  0x000A0000
 Sets Interrupt on either edge.
		#define INTR_EITHER_EDGE   0x000B0000
 Sets Interrupt when logic 1.
		#define INTR_LOGIC1        0x000C0000
 * */

/* brief: enable both sw2 and sw3
 * param[in]:
 * param[out]: void
 * */
void enable_both_switches();

/* brief: enable  sw2
 * param[in]:
 * param[out]: void
 * */
void init_sw2(priority_level_t priority_threshold , priority_level_t priority, uint32_t interruptEgde );

/* brief: enable both sw3
 * param[in]:
 * param[out]: void
 * */
void init_sw3(priority_level_t priority_threshold , priority_level_t priority, uint32_t interruptEgde );

/* brief: enable  sw2
 * param[in]:
 * param[out]: void
 * */
void disable_sw2();

/* brief: enable both sw3
 * param[in]:
 * param[out]: void
 * */
void disable_sw3();

uint32_t sw2_pressed();
uint32_t sw3_pressed();
uint32_t sw2_one_shot();//if it was , then  clears his flag, button state is processed
uint32_t sw3_one_shot();

#endif /* SWITCHES_K64_H_ */

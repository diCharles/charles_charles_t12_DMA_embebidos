/*
 * switches_k64.c
 *
 *  Created on: 09/03/2019
 *      Author: Charles
 */
#include "switches_k64.h"
/* flag set by sw2 pressed, sw2_one_shot and handler for PORTC irq (contained on GPIO.c)
 * also the flag is cleared only by sw3_one_shot  */
 volatile uint8_t g_sw2_was_pressed_flag=FALSE;

/* flag set by sw3 pressed, sw3_one_shot and hander for PORTA irq (contained on GPIO.c)
 * also the flag is cleared only by sw3_one_shot  */
 volatile uint8_t  g_sw3_was_pressed_flag=FALSE;

void enable_both_switches();



void init_sw2(priority_level_t priority_threshold , priority_level_t priority, uint32_t interruptEgde )
{
	uint32_t input_intr_config = GPIO_MUX1|GPIO_PE|GPIO_PS;
	if(0 != (priority_threshold && priority))
	{
		/**Sets the threshold for interrupts, if the interrupt has higher priority constant that the BASEPRI, the interrupt will not be attended*/
		NVIC_set_basepri_threshold(priority_threshold);
		/**Enables and sets a particular interrupt and its priority*/
		NVIC_enable_interrupt_and_priotity(PORTC_IRQ,priority);
		input_intr_config|=interruptEgde;
		NVIC_global_enable_interrupts;
	}
	GPIO_clock_gating(GPIO_C);
	GPIO_pin_control_register(GPIO_C, bit_6, &input_intr_config);
	GPIO_data_direction_pin(GPIO_C, GPIO_PIN_INPUT , bit_6);
}


void init_sw3(priority_level_t priority_threshold , priority_level_t priority, uint32_t interruptEgde )
{
	uint32_t input_intr_config = GPIO_MUX1|GPIO_PE|GPIO_PS;
	if(0 != (priority_threshold && priority))
	{
		/**Sets the threshold for interrupts, if the interrupt has higher priority constant that the BASEPRI, the interrupt will not be attended*/
		NVIC_set_basepri_threshold(priority_threshold);
		/**Enables and sets a particular interrupt and its priority*/
		NVIC_enable_interrupt_and_priotity(PORTA_IRQ,priority);
		input_intr_config|=interruptEgde;
		NVIC_global_enable_interrupts;
	}
	GPIO_clock_gating(GPIO_A);
	GPIO_pin_control_register(GPIO_A, bit_4, &input_intr_config);
	GPIO_data_direction_pin(GPIO_A, GPIO_PIN_INPUT, bit_4);
}



uint32_t sw2_pressed()
{
	if( TRUE ==GPIO_read_pin(GPIO_C, 6))
		{
			//do not change pressed flag
			return FALSE;
		}
	g_sw2_was_pressed_flag=TRUE;
	return TRUE;
}
uint32_t sw3_pressed()
{
	if( (FALSE ==GPIO_read_pin(GPIO_A, 4)) )
	{
		//do not change pressed flag
		g_sw3_was_pressed_flag=TRUE;
		return TRUE;
	}

	//do not change pressed flag
	return FALSE;
}
uint32_t sw2_one_shot()
{
	static uint8_t one_shot=TRUE;
	if((TRUE ==g_sw2_was_pressed_flag) && (TRUE ==one_shot))
	{
		g_sw2_was_pressed_flag=FALSE;
		one_shot=FALSE;
		return TRUE;
	}
	if(TRUE ==g_sw2_was_pressed_flag )
	{
		one_shot=FALSE;
		g_sw2_was_pressed_flag=FALSE;
		return FALSE;
	}
	else if( FALSE ==g_sw2_was_pressed_flag)
	{
		one_shot=TRUE;
	}
	return FALSE;

}
uint32_t sw3_one_shot()
{
	static uint8_t one_shot=TRUE;
	if((TRUE ==g_sw3_was_pressed_flag) && (TRUE ==one_shot))
	{
		g_sw3_was_pressed_flag=FALSE;
		one_shot=FALSE;
		return TRUE;
	}
	if(TRUE ==g_sw3_was_pressed_flag )
	{
		one_shot=FALSE;
		g_sw3_was_pressed_flag=FALSE;
		return FALSE;
	}
	else if( FALSE ==g_sw3_was_pressed_flag)
	{
		one_shot=TRUE;
	}
	return FALSE;

}

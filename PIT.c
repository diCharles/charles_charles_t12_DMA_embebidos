
#include "PIT.h"
#include "Bits.h"
#include "GPIO.h"
static uint8_t g_pit_0_intr_flag = FALSE;
static uint8_t g_pit_1_intr_flag = FALSE;
static uint8_t g_pit_2_intr_flag = FALSE;
static uint8_t g_pit_3_intr_flag = FALSE;

void PIT0_IRQHandler()
{
	volatile uint32_t dummyRead;

	PIT->CHANNEL[0].TFLG |= PIT_TFLG_TIF_MASK;
	dummyRead = PIT->CHANNEL[0].TCTRL;	//read control register for clear PIT flag, this is silicon bug


	g_pit_0_intr_flag = TRUE;
}
void PIT1_IRQHandler()
{
	volatile uint32_t dummyRead;

	PIT->CHANNEL[1].TFLG |= PIT_TFLG_TIF_MASK;
	dummyRead = PIT->CHANNEL[1].TCTRL;	//read control register for clear PIT flag, this is silicon bug


	g_pit_1_intr_flag = TRUE;
}
void PIT2_IRQHandler()
{
	volatile uint32_t dummyRead;

	PIT->CHANNEL[2].TFLG |= PIT_TFLG_TIF_MASK;
	dummyRead = PIT->CHANNEL[2].TCTRL;	//read control register for clear PIT flag, this is silicon bug


	g_pit_2_intr_flag = TRUE;
}
void PIT3_IRQHandler()
{
	volatile uint32_t dummyRead;

	PIT->CHANNEL[3].TFLG |= PIT_TFLG_TIF_MASK;
	dummyRead = PIT->CHANNEL[3].TCTRL;	//read control register for clear PIT flag, this is silicon bug


	g_pit_3_intr_flag = TRUE;
}
void PIT_delay(PIT_timer_t pit_timer,My_float_pit_t system_clock , My_float_pit_t delay)
{
	uint32_t LDVAL = 0;
	My_float_pit_t clock_period = 0.0F;
	system_clock = system_clock /2;
	clock_period = (1/system_clock);
	LDVAL = (uint32_t)(delay / clock_period);
	LDVAL = LDVAL - 1;

	switch(pit_timer)
	{
	case PIT_0:
		PIT->CHANNEL[0].LDVAL = LDVAL;
		PIT_enable_interrupt(PIT_0);
		break;
	case PIT_1:
			PIT->CHANNEL[1].LDVAL = LDVAL;
			PIT_enable_interrupt(PIT_1);
			break;
	case PIT_2:
			PIT->CHANNEL[2].LDVAL = LDVAL;
			PIT_enable_interrupt(PIT_2);
			break;
	case PIT_3:
			PIT->CHANNEL[3].LDVAL = LDVAL;
			PIT_enable_interrupt(PIT_3);
			break;
	default:

		break;
	}


}


void PIT_enable(void)
{
	PIT->MCR  |= PIT_MCR_FRZ_MASK;
	PIT->MCR &= ~PIT_MCR_MDIS_MASK; /* Enable PIT*/
}

void PIT_enable_interrupt(PIT_timer_t pit)
{
	PIT->CHANNEL[pit].TCTRL |= PIT_TCTRL_TIE_MASK;//enables PIT timer interrupt
	PIT->CHANNEL[pit].TCTRL |= PIT_TCTRL_TEN_MASK;//enables timer0
}

void PIT_clock_gating(void)
{
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK;
}

uint8_t PIT_get_interrupt_flag_status(PIT_timer_t pit)
{
	switch (pit)
	{
	case PIT_0:
		return(g_pit_0_intr_flag);
			break;
	case PIT_1:
			return(g_pit_1_intr_flag);
				break;
	case PIT_2:
			return(g_pit_2_intr_flag);
				break;
	case PIT_3:
			return(g_pit_3_intr_flag);
				break;
	default:
		break;
	}
	return 0 ;
}

void PIT_clear_interrupt_flag(PIT_timer_t pit)
{
	switch (pit)
		{
		case PIT_0:
			g_pit_0_intr_flag= FALSE;
				break;
		case PIT_1:
				g_pit_1_intr_flag=FALSE;
					break;
		case PIT_2:
				g_pit_2_intr_flag=FALSE;
					break;
		case PIT_3:
				g_pit_3_intr_flag=FALSE;
					break;
		default:
			break;
		}
}

void set_PIT_timer_with_interrupt(PIT_timer_t pit,My_float_pit_t system_clock , My_float_pit_t delay,
									interrupt_t interrupt_number, priority_level_t priority)
{
	PIT_clock_gating();
	PIT_enable();
	NVIC_enable_interrupt_and_priotity(interrupt_number, priority);
	NVIC_global_enable_interrupts;
	PIT_delay(pit, system_clock, delay);
}

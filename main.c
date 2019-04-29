#include "MK64F12.h"
#include "NVIC.h"
#include "GPIO.h"
#include "Bits.h"
#include <stdio.h>
#include "generador.h"

#define DEBUG_PRINT 0u  /** prints debug messages about pit with interrupt and DAC value from DMA transfer*/
#define DEBUG_DMA_SOUCE_SW3 1u/** selects sw2 as source of DMA channel 0*/

#define SYSTEM_CLOCK (21000000u)
#define DELAY (0.01F)
#define ARRAY_SIZE (16u)

#define DMA_CH0 (0x01u)
#define DMA_SOURCE_GPIO (51u)
#define DMA_SOURCE_ALWAYS_ENABLED (63u)


uint16_t g_data_source[ARRAY_SIZE] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};//defines source data space
uint16_t g_data_desti[4*ARRAY_SIZE]; //defines destination data s
uint16_t g_DAC_output = 0;

/** functions for DMA*/
void DMA_clock_gating(void);
void DMA_init(void);
/** function for DAC*/
void init_DAC();

/** functions for ADC*/

int main(void)
{
	gpio_pin_control_register_t sw2 = GPIO_MUX1 | GPIO_PE | GPIO_PS | DMA_FALLING_EDGE; /* GPIO configured to trigger the DMA*/
	GPIO_clock_gating(GPIO_C);
	GPIO_pin_control_register(GPIO_C, bit_6, &sw2);

	DMA_clock_gating();
	DMA_init(); /* Configure the DMA*/
	NVIC_enable_interrupt_and_priotity(DMA_CH0_IRQ, PRIORITY_5);
	NVIC_global_enable_interrupts;
	init_DAC();
	init_generador_seniales();
	for (;;)
	{
		generador_seniales();
	#ifdef DEBUG
		if(PIT_get_interrupt_flag_status(PIT_0))
		{
			uint16_t DAC_output = 	(uint16_t)DAC0->DAT[0].DATH <<8  |	(uint16_t)DAC0->DAT[0].DATL  ;
			printf("pit is here baby, the dac receives %i\n",DAC_output);
			PIT_clear_interrupt_flag(PIT_0);
		}
	#endif

	}
	/* Never leave main */
	return 0;
}
void DMA_clock_gating(void)
{
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
}
void DMA_init(void)
{
	/** PARAMETERS TO MODIFY*/
	uint8_t word_width_in_bytes = 2;
	uint32_t 	words_to_trasmmit =  1  * word_width_in_bytes;
	uint32_t citer_steps = 1;

	uint32_t source_offset = 1; /** write this on words, a word not from k64 archq , a word from DMA transfer*/
	uint32_t destination_offset = 1 /** write this on words, , a word not from k64 archq , a word from DMA transfer */;

	DMAMUX->CHCFG[2] = 0;
	DMAMUX->CHCFG[2] = DMAMUX_CHCFG_ENBL_MASK  /*enables DMA MUX channel*/
#ifdef DMA_SOURCE_ALWAYS_ENABLED
			|DMAMUX_CHCFG_TRIG_MASK |/** DMA trigger for channel 0 is PIT0, view pag  458*/
			DMAMUX_CHCFG_SOURCE(DMA_SOURCE_ALWAYS_ENABLED);
#endif
#ifndef DMA_SOURCE_ALWAYS_ENABLED
			|DMAMUX_CHCFG_SOURCE(DMA_SOURCE_GPIO);
#endif

	DMA0->ERQ = 0x01;//enables DMA0 request

	DMA0->TCD[0].SADDR = (uint32_t)(&g_data_source[0]);/*defines source data address*/
	DMA0->TCD[0].SOFF =  source_offset *word_width_in_bytes;/*Source address signed offset;it is expressed in number of bytes*/


	DMA0->TCD[0].DADDR = (uint32_t)(&DAC0->DAT[0].DATL);/*defines destination data address*/
	DMA0->TCD[0].DOFF = destination_offset;/*destination address signed offset;it is expressed in number of bytes*/

	DMA0->TCD[0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(citer_steps);
	/* Once a major loop is completed, BITTER is copy to CITTER*/
	DMA0->TCD[0].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER( citer_steps );
	DMA0->TCD[0].NBYTES_MLNO = words_to_trasmmit ;/*byte number for transference*/

	DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(word_width_in_bytes-1);/*16 bit transfer size, in order to transfer see Kinetis user manual*/
	DMA0->TCD[0].SLAST = 0 ;//restores the source address to the initial value, which is expressed in the amount of bytes to restore*/
	DMA0->TCD[0].DLAST_SGA =  -2;/*restores the destination address to the initial value, which is expressed in the amount of bytes to restore*/
	DMA0->TCD[0].CSR = DMA_CSR_INTMAJOR_MASK;/*The end-of-major loop interrupt is enabled*/


}
void DMA0_IRQHandler(void)
{


	DMA0->INT = DMA_CH0;
#ifdef DEBUG_PRINT
	uint16_t DAC_output = 	(uint16_t)DAC0->DAT[0].DATH <<8  |	(uint16_t)DAC0->DAT[0].DATL  ;
	printf(" DAC vaalue %i\n",DAC_output );
#endif
	if(DMA0->TCD[0].SADDR == (uint32_t)(&g_data_source[ARRAY_SIZE]))
	{
		DMA0->TCD[0].SADDR = (uint32_t)(&g_data_source[0]);/*defines source data address*/
	}
}

void init_DAC()
{
	SIM->SCGC2 = 0x1000;
	DAC0->C0 = 0xC0;
	DAC0->DAT[0].DATL = 0;
	DAC0->DAT[0].DATH = 0;

}

////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

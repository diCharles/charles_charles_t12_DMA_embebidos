#include "MK64F12.h"
#include "NVIC.h"
#include "GPIO.h"
#include "Bits.h"
#include <stdio.h>


#define SYSTEM_CLOCK (21000000u)
#define DELAY (0.01F)

#define ARRAY_SIZE (16u)

#define DMA_CH0 (0x01u)
#define DMA_SOURCE_GPIO (51u)

uint16_t g_data_source[ARRAY_SIZE] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};//defines source data space
uint16_t g_data_desti[4*ARRAY_SIZE]; //defines destination data space


void DMA0_IRQHandler(void)
{

	uint8_t i;

	DMA0->INT = DMA_CH0;

	for ( i = 0; i < ARRAY_SIZE; ++i)
	{
		printf("%d,",g_data_desti[i]);
	}
	printf("\n");
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
	uint32_t 	words_to_trasmmit = 16 * word_width_in_bytes;
	uint32_t citer_steps = 1;
	uint32_t source_offset = 1; /** write this on words, a word not from k64 archq , a word from DMA transfer*/
	uint32_t destination_offset = 1 /** write this on words, , a word not from k64 archq , a word from DMA transfer */;

	DMAMUX->CHCFG[0] = 0;
	DMAMUX->CHCFG[0] = DMAMUX_CHCFG_ENBL_MASK | /*enables DMA MUX channel*/
					   DMAMUX_CHCFG_SOURCE(DMA_SOURCE_GPIO);/*source is GPIO PIN  corresponding to sw2*/

	DMA0->ERQ = 0x01;//enables DMA0 request

	DMA0->TCD[0].SADDR = (uint32_t)(&g_data_source[0]);/*defines source data address*/
	DMA0->TCD[0].SOFF =  source_offset *word_width_in_bytes;/*Source address signed offset;it is expressed in number of bytes*/
	DMA0->TCD[0].DADDR = (uint32_t)(&g_data_desti[0]);/*defines destination data address*/
	DMA0->TCD[0].DOFF = destination_offset;/*destination address signed offset;it is expressed in number of bytes*/
	/*CITER represents the number of minor loops that compose a major loop, every time a minor loop is completed CITTER is decremented by one.
	 * Once CITTER is 0 the DMA copy BITTER into CITTER and adjust SADDR and DADDR with the values specified in SLAST and DLAST_SGA respectively
	 *  SADDR  = SADDR + SLAST and DADDR + DLAST_SGA*/


	DMA0->TCD[0].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(citer_steps);
	/* Once a major loop is completed, BITTER is copy to CITTER*/
	DMA0->TCD[0].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER( citer_steps );
	DMA0->TCD[0].NBYTES_MLNO = (words_to_trasmmit*word_width_in_bytes ) / citer_steps ;/*byte number for transference*/

	DMA0->TCD[0].ATTR = DMA_ATTR_SSIZE(word_width_in_bytes-1);/*16 bit transfer size, in order to transfer see Kinetis user manual*/
	DMA0->TCD[0].SLAST = -words_to_trasmmit*word_width_in_bytes;//restores the source address to the initial value, which is expressed in the amount of bytes to restore*/
	DMA0->TCD[0].DLAST_SGA = - (words_to_trasmmit * word_width_in_bytes * destination_offset );/*restores the destination address to the initial value, which is expressed in the amount of bytes to restore*/
	DMA0->TCD[0].CSR = DMA_CSR_INTMAJOR_MASK;/*The end-of-major loop interrupt is enabled*/


}

int main(void)
{
	gpio_pin_control_register_t sw2 = GPIO_MUX1 | GPIO_PE | GPIO_PS | DMA_FALLING_EDGE; /* GPIO configured to trigger the DMA*/

	GPIO_clock_gating(GPIO_C);
	GPIO_pin_control_register(GPIO_C, bit_6, &sw2);
	DMA_clock_gating();
	DMA_init(); /* Configure the T*/
	NVIC_enable_interrupt_and_priotity(DMA_CH0_IRQ, PRIORITY_5);
	NVIC_global_enable_interrupts;
    for (;;) {

    	/** only for debug porpuses*/
    	uint32_t source_address = DMA0->TCD[0].SADDR;
    	uint32_t desti_address = DMA0->TCD[0].DADDR;
    }
    /* Never leave main */
    return 0;
}
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////

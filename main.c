#include "MK64F12.h"
#include "NVIC.h"
#include "GPIO.h"
#include "Bits.h"
#include <stdio.h>
#include "generador.h"

//#define DEBUG_PRINT 0u  /** prints debug messages about pit with interrupt and DAC value from DMA transfer*/
#define DEBUG_DMA_SOUCE_SW3 1u/** selects sw2 as source of DMA channel 0*/

#define SYSTEM_CLOCK (21000000u)

#define ARRAY_SIZE (16u)
#define LUT_VALUES 161u
#define INITIAL_LUT_VALUE 0u
#define GEN_SQUARE 0u
#define GEN_TRIANGULAR 1u
#define GEN_SINE       2u

#define DMA_CH0 (0x01u)
#define DMA_SOURCE_GPIO (51u)/** sw2 is the source for a new dma transfer*/
#define DMA_SOURCE_ALWAYS_ENABLED (63u)/** if not commented the pit will trigger DMA*/


uint16_t g_sin_lut[LUT_VALUES] = {
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
		2000};

uint16_t g_triangular_lut[LUT_VALUES] =
{
		0,  50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950,1000,1050,1100,1150,
		1200,1250,1300,1350,1400,1450,1500,1550,1600,1650,1700,1750,1800,1850,1900,1950,2000,2050,2100,2150,2200,2250,2300,2350,
		2400,2450,2500,2550,2600,2650,2700,2750,2800,2850,2900,2950,3000,3050,3100,3150,3200,3250,3300,3350,3400,3450,3500,3550,
		3600,3650,3700,3750,3800,3850,3900,3950,4000,3950,3900,3850,3800,3750,3700,3650,3600,3550,3500,3450,3400,3350,3300,3250,
		3200,3150,3100,3050,3000,2950,2900,2850,2800,2750,2700,2650,2600,2550,2500,2450,2400,2350,2300,2250,2200,2150,2100,2050,
		2000,1950,1900,1850,1800,1750,1700,1650,1600,1550,1500,1450,1400,1350,1300,1250,1200,1150,1100,1050,1000, 950, 900, 850,
		800, 750, 700, 650, 600, 550,500, 450,  400, 350, 300, 250, 200, 150, 100,  50,  0
};
uint16_t g_square_lut[LUT_VALUES] =
{
		3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
		3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,3000,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};



/** functions for DMA*/
void DMA_clock_gating(void);
void DMA_init(void);
/** function for DAC*/
void init_DAC();



int main(void)
{
#ifndef DMA_SOURCE_ALWAYS_ENABLED
	gpio_pin_control_register_t sw2 = GPIO_MUX1 | GPIO_PE | GPIO_PS | DMA_FALLING_EDGE; /* GPIO configured to trigger the DMA*/
	GPIO_clock_gating(GPIO_C);
	GPIO_pin_control_register(GPIO_C, bit_6, &sw2);
#endif
	DMA_clock_gating();
	DMA_init(); /* Configure the DMA*/

	init_DAC();
	init_generador_seniales();
	for (;;)
	{


		static uint8_t sw3_counter=0;
		/* if the sw3 is pressed a corresponding flag interrupt will be set*/
		if (1 == GPIO_get_irq_status(GPIO_A))
		{
			sw3_counter++;						/* every press will increment the state of the machine*/
			switch (sw3_counter)
			{
			case 1:
				DMA0->TCD[0].SADDR = (uint32_t )&(g_square_lut[INITIAL_LUT_VALUE]);
				break;
			case 2:
				DMA0->TCD[0].SADDR = (uint32_t)&(g_triangular_lut[INITIAL_LUT_VALUE]);
				break;
			case 3:
				DMA0->TCD[0].SADDR = (uint32_t)&(g_sin_lut[INITIAL_LUT_VALUE]);
				break;

			}
			GPIO_clear_irq_status(GPIO_A);		/*flag must be cleared, if it not the state machine will change when it shouldn't*/
		}
		/*very important to check overflow*/
		if( NUMBER_OF_STATES <=sw3_counter)
		{
			sw3_counter = GEN_SQUARE ;
		}

		if(( DMA0->TCD[0].SADDR >= (uint32_t)(&g_square_lut[LUT_VALUES-4])) && GEN_SQUARE ==sw3_counter)
		{
			DMA0->TCD[0].SADDR =(uint32_t) &(g_square_lut[INITIAL_LUT_VALUE]);/*defines source data address*/
		}
		else if(( DMA0->TCD[0].SADDR >= (uint32_t)(&g_triangular_lut[LUT_VALUES-4])) &&  GEN_TRIANGULAR ==sw3_counter)
		{
			DMA0->TCD[0].SADDR =(uint32_t) &(g_triangular_lut[INITIAL_LUT_VALUE]);/*defines source data address*/
		}
		else if(( DMA0->TCD[0].SADDR >= (uint32_t)(&g_sin_lut[LUT_VALUES-4])) && (GEN_SINE  ==sw3_counter) )
		{
			DMA0->TCD[0].SADDR =(uint32_t) &(g_sin_lut[INITIAL_LUT_VALUE]);/*defines source data address*/
		}



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

	DMAMUX->CHCFG[0] = 0;
	DMAMUX->CHCFG[0] = DMAMUX_CHCFG_ENBL_MASK ; /*enables DMA MUX channel*/
#ifdef DMA_SOURCE_ALWAYS_ENABLED
	//DMAMUX->CHCFG[0] &= ~ (DMAMUX_CHCFG_TRIG_MASK);
	DMAMUX->CHCFG[0]|= DMAMUX_CHCFG_TRIG_MASK |/** DMA trigger for channel 0 is PIT0, view pag  458*/
			DMAMUX_CHCFG_SOURCE(DMA_SOURCE_ALWAYS_ENABLED);
#endif
#ifndef DMA_SOURCE_ALWAYS_ENABLED
	DMAMUX->CHCFG[0] |= DMAMUX_CHCFG_SOURCE(DMA_SOURCE_GPIO);
#endif

	DMA0->ERQ = 0x01;//enables DMA0 request

	DMA0->TCD[0].SADDR = (uint32_t )&(g_square_lut[INITIAL_LUT_VALUE]);
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
	//DMA0->TCD[0].CSR = DMA_CSR_INTMAJOR_MASK;/*The end-of-major loop interrupt is enabled*/


}
void DMA0_IRQHandler(void)
{


	DMA0->INT = DMA_CH0;
#ifdef DEBUG_PRINT
	uint16_t DAC_output = 	(uint16_t)DAC0->DAT[0].DATH <<8  |	(uint16_t)DAC0->DAT[0].DATL  ;
	printf(" DAC vaalue %i\n",DAC_output );

#endif
	printf(" DMA interruput\n");


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

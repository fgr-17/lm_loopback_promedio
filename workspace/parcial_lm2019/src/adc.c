/*
 * adc.c
 *
 *  Created on: 26 jun. 2019
 *      Author: froux
 */

/* ========================================= inclusion de archivos ========================================= */

#include "board.h"
#include <cr_section_macros.h>

#include "tipos.h"

#include "adc.h"


/* ========================================= prototipos de funciones ========================================= */

int InicializarTimer (void);
int InicializarADCporIRQ (void);
uint16_t PromedioBuf (uint16_t*pBuf, uint32_t xL);
/* ========================================= variables globales ========================================= */


/* ------------------- flags de estado de buffers ping pong ----------------- */
pp_t estadoADCpp;

/* ----------------- buffers de datos para adc ----------------------- */
uint16_t adcBufPing [ADC_BUF_L];
uint16_t adcBufPong [ADC_BUF_L];

uint16_t*pBufEscritura;
uint16_t*pBufLectura;

uint32_t indBufEscritura;
uint32_t indBufLectura;

uint8_t flagProcesar = 0;

ADC_CLOCK_SETUP_T ADCSetup;


/* ========================================= implementación de funciones ========================================= */


/**
==============================================================
@fn 		int InicializarBuffersADC (void)
@brief		inicializacion del buffers e indices
@paramin 	void
@paramout 	0 success
@author 	fede(rouxfederico@gmail.com)
==============================================================
*/
int InicializarBuffersADC (void) {

	pBufEscritura = adcBufPing;
	pBufLectura = adcBufPong;

	indBufEscritura = 0;
	indBufLectura = 0;

	flagProcesar = 0;

	return 0;
}



/**
==============================================================
@fn 		int InicializarTimer (void)
@brief		inicializacion del timer para disparar el adc
@paramin 	void
@paramout 	0 success
@author 	fede(rouxfederico@gmail.com)
==============================================================
*/

int InicializarTimer (void) {

	uint32_t timerFreq;

	/* selecciono el divisor PCLK en 1 */
	Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_TIMER0, 01);

	/* Enable timer 1 clock */
	Chip_TIMER_Init(LPC_TIMER0);

	/* Timer rate is system clock rate */
	timerFreq = Chip_Clock_GetSystemClockRate();

	/* Timer setup for match and interrupt at TICKRATE_HZ */
	Chip_TIMER_Reset(LPC_TIMER0);
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, TIMER0_MATCH_REG);
	Chip_TIMER_SetMatch(LPC_TIMER0, TIMER0_MATCH_REG, (timerFreq / TICKRATE_HZ1));
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, TIMER0_MATCH_REG);

	// esto es muy importante si quiero disparar el adc desde el timer
	Chip_TIMER_ExtMatchControlSet(LPC_TIMER0, 0, TIMER_EXTMATCH_TOGGLE, TIMER0_MATCH_REG);
	Chip_TIMER_Enable(LPC_TIMER0);

	/* Enable timer interrupt */
	NVIC_ClearPendingIRQ(TIMER0_IRQn);
	NVIC_EnableIRQ(TIMER0_IRQn);

	return 0;
}

/**
==============================================================
@fn 		int InicializarADCporIRQ (void)
@brief		inicializacion del ADC a traves del DMA
@paramin 	void
@paramout 	0 success
@author 	fede(rouxfederico@gmail.com)
==============================================================
*/

int InicializarADCporIRQ (void) {


	InicializarBuffersADC();

	/*ADC Init */
// 	Chip_IOCON_PinMux(LPC_IOCON, 0, 23, IOCON_MODE_INACT, IOCON_FUNC1);
	Chip_ADC_Init(LPC_ADC, &ADCSetup);
	Chip_ADC_EnableChannel(LPC_ADC, ADC_CH0, ENABLE);

	/* -- habilitacion por irq -- */
	/* Enable ADC Interrupt */
	Chip_ADC_Int_SetChannelCmd(LPC_ADC, ADC_CH0, ENABLE);
	Chip_ADC_SetStartMode(LPC_ADC, ADC_START_ON_ADCTRIG0, ADC_TRIGGERMODE_RISING);

	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);


	return 0;
}
/* ========================================= handler de interrupción ========================================= */

/**
===================================================
 @fn 		void TIMER0_IRQHandler(void)
 @brief		Handle interrupt from 32-bit timer
 @return	Nothing
===================================================
*/
void TIMER0_IRQHandler(void)
{
	if (Chip_TIMER_MatchPending(LPC_TIMER0, TIMER0_MATCH_REG)) {
		Chip_TIMER_ClearMatch(LPC_TIMER0, TIMER0_MATCH_REG);
	}
}

/**
===================================================
 @fn 		void ADC_IRQHandler(void)
 @brief		ADC0 interrupt handler sub-routine
 @return	Nothing
===================================================
 */
void ADC_IRQHandler(void)
{
	uint16_t dataADC;
	uint16_t*pAux;

	/* Interrupt mode: Call the stream interrupt handler */
	NVIC_ClearPendingIRQ(ADC_IRQn);

	if(Chip_ADC_ReadStatus(LPC_ADC, ADC_CH0, ADC_DR_DONE_STAT) ) {
		Chip_ADC_ReadValue(LPC_ADC, ADC_CH0, &dataADC);

		pBufEscritura[indBufEscritura] = dataADC;
		indBufEscritura++;

		if(indBufEscritura >= ADC_BUF_L) {
			indBufEscritura = 0;
			pAux = pBufEscritura;
			pBufEscritura = pBufLectura;
			pBufLectura = pAux;
			flagProcesar = 1;
		}
	}

	return;
}


uint16_t PromedioBuf (uint16_t*pBuf, uint32_t xL) {

	uint32_t sum = 0;
	uint16_t prom;
	uint32_t i;

	for(i = 0; i < xL; i++) {

		sum += pBuf[i];
	}

	prom = sum >> 10;


	return prom;
}



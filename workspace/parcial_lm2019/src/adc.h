/*
 * adc.h
 *
 *  Created on: 26 jun. 2019
 *      Author: froux
 */

#ifndef ADC_H_
#define ADC_H_
/* ========================================= declaración de constantes ========================================= */

#define ADC_BUF_L	 1024

#define TICKRATE_HZ1 (1000)	/* 11 ticks per second */
#define TIMER0_MATCH_REG	1


/* ========================================= funciones externas ========================================= */

extern int InicializarTimer (void);
extern int InicializarADCporIRQ (void);
uint16_t PromedioBuf (uint16_t*pBuf, uint32_t xL);

/* ========================================= variables externas ========================================= */

extern uint8_t flagProcesar;
extern uint16_t*pBufEscritura;
extern uint16_t*pBufLectura;


#endif /* ADC_H_ */

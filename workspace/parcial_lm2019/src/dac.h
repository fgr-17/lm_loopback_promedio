/*
 * dac.h
 *
 *  Created on: 26 jun. 2019
 *      Author: froux
 */

#ifndef DAC_H_
#define DAC_H_
/* ========================================= declaración de constantes ========================================= */

/* DAC sample rate request time maximo 16 bits*/
#define DAC_FS				100

#define DAC_AOUT_PORT		0
#define DAC_AOUT_PIN		26

#define DAC_BUF_L	 		128

/* ========================================= funciones externas ========================================= */

extern int32_t InicializarDAC (void);
extern int32_t TareaActualizarSalidaDAC (void);

extern uint32_t salidaDAC;

#endif /* DAC_H_ */

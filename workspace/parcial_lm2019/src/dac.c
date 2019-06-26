/*
 * dac.c
 *
 *  Created on: 26 jun. 2019
 *      Author: froux
 */

/* ========================================= inclusion de archivos ========================================= */

#include "board.h"
#include <cr_section_macros.h>

#include "tipos.h"

#include "dac.h"

/* ========================================= prototipos de funciones ========================================= */

int32_t InicializarDAC (void);
int32_t ActualizarSalidaDAC (void);

/* ========================================= variables globales ========================================= */

/* ------------------- puntero a buffer de salida del dac ----------------- */

uint32_t salidaDAC = 0;


/* ========================================= implementación de funciones ========================================= */

/**
 ====================================================
 @fn int32_t InicializarDAC (void)
 @brief Inicializa el dac disparado por dma con doble buffer
 @paramin void
 @paramout 0 sucess
 @author fede (rouxfederico@gmail.com)
 ====================================================
*/

int32_t InicializarDAC (void) {

	uint32_t dacClk;

	/* antes de modificar los registros, poner la funcion DAC en el pin AOUT UM10360 30.1.3*/
	Chip_IOCON_PinMuxSet(LPC_IOCON, DAC_AOUT_PORT, DAC_AOUT_PIN, IOCON_MODE_INACT | IOCON_FUNC2);

	/* Setup DAC pins for board and common CHIP code */
	Chip_DAC_Init(LPC_DAC);
	Chip_Clock_SetPCLKDiv(SYSCTL_PCLK_DAC, SYSCTL_CLKDIV_1);

	dacClk = Chip_Clock_GetPeripheralClockRate(SYSCTL_PCLK_DAC);

	Chip_DAC_SetDMATimeOut(LPC_DAC, dacClk/DAC_FS);
	Chip_DAC_ConfigDAConverterControl(LPC_DAC, DAC_CNT_ENA | DAC_DMA_ENA);


	return 0;
}

/**
 ====================================================
 @fn int32_t ActualizarSalidaDAC (void)
 @brief actualizo la salida DAC si esta disponible
 @paramin void
 @paramout 0 sucess
 @author fede (rouxfederico@gmail.com)
 ====================================================
 */

int32_t TareaActualizarSalidaDAC (void)
{
	static uint32_t i = 0;

	uint16_t x;

	if(Chip_DAC_GetIntStatus(LPC_DAC)) {
		/* desplazo salidaDAC dos bits hacia la derecha porque el adc es de 12 y el dac es de 10 */
		Chip_DAC_UpdateValue(LPC_DAC, salidaDAC >> 2);
		return 0;
	}
	else
		return -1;

}

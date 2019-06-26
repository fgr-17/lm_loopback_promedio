/*
===============================================================================
 Name        : loopback.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

/*========================== inclusion de archivos ==========================*/
#include <string.h>
#include "board.h"
#include <cr_section_macros.h>

#include "adc.h"
#include "dac.h"


/*========================== prototipos de fcs ==========================*/

int InicializarUART (void);
uint32_t tareaProcesar (void);
/*========================== variables globales ==========================*/


char mensajeBienvenida[] = "LM2019: prueba del ADC \r\n";


/*========================== implementacion de fcs ==========================*/

/**
==============================================================
@fn 		int main(void)
@brief 		programa ppal
@paramin 	void
@paramout 	0 success
@author 	fede(rouxfederico@gmail.com)
==============================================================
*/

int main(void) {

    SystemCoreClockUpdate();
    Board_Init();
    Board_LED_Set(0, true);

    InicializarUART();
    InicializarADCporIRQ();
    InicializarTimer();
    InicializarDAC();

    while(1) {
    	TareaActualizarSalidaDAC();
    	tareaProcesar();
    	__WFI();
    }
    return 0 ;
}

/**
==============================================================
@fn 		int InicializarUART (void)
@brief 		inicializacion de la uart para debug y manejo
@paramin 	void
@paramout 	0 success
@author 	fede(rouxfederico@gmail.com)
==============================================================
*/

int InicializarUART (void) {

    Chip_UART_Init(LPC_UART3);
	Chip_UART_SetBaud(LPC_UART3, 115200);
	Chip_UART_ConfigData(LPC_UART3, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(LPC_UART3, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV1));
	Chip_UART_TXEnable(LPC_UART3);
    Chip_UART_SendBlocking(LPC_UART3, mensajeBienvenida, sizeof(mensajeBienvenida) - 1);

    return 0;

}

uint32_t tareaProcesar (void) {

	if(flagProcesar) {
		flagProcesar = 0;
		salidaDAC = PromedioBuf(pBufLectura, ADC_BUF_L);
	}
	return 0;
}

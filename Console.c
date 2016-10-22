/*
 * Console.c
 *
 *  Created on: Oct 13, 2016
 *      Author: eric
 */

#include <stdarg.h>
#include <stdio.h>

#include "Console.h"
#include "hw_memmap.h"
#include "pin_mux_config.h"
#include "hw_types.h"
#include "rom_map.h"
#include "uart.h"
#include "prcm.h"

#define UART_ADDR	UARTA0_BASE


void Console_init(uint32_t _baud) {
	//Initialize the UART
	UARTConfigSetExpClk(UART_ADDR,
		MAP_PRCMPeripheralClockGet(PRCM_UARTA0),
		_baud,
		UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);

	//Enable the FIFO
	UARTFIFOEnable(UART_ADDR);

	//Set UART FIFO level to 1/2
	UARTFIFOLevelSet(UART_ADDR, UART_FIFO_TX4_8, UART_FIFO_RX4_8);

	//Register interrupt handler
	//UARTIntRegister(UART_ADDR, uart_intHandler);

	//Enable RX and RT interrupts
	//UARTIntEnable(UART_ADDR, UART_INT_RT | UART_INT_RX);
}

void Console_puts(const char* _str) {
	while(*_str != '\0')
		UARTCharPut(UART_ADDR, *(_str++));
}

void Console_printf(const char* format, ... ) {
	char buffer[256];

	va_list args;
	va_start(args, format);

	vsprintf(buffer, format, args);

	Console_puts(buffer);

	va_end(args);
}

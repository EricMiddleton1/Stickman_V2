/*
 * Console.h
 *
 *  Created on: Oct 13, 2016
 *      Author: eric
 */

#ifndef CONSOLE_H_
#define CONSOLE_H_

#include <stdint.h>

void Console_init(uint32_t _baud);

void Console_puts(const char*);

void Console_printf(const char*, ... );


#endif /* CONSOLE_H_ */

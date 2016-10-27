/*
 * MSGEQ7.h
 *
 *  Created on: Oct 20, 2016
 *      Author: eric
 */

#ifndef MSGEQ7_H_
#define MSGEQ7_H_

#include <stdint.h>

#define GEQ_CH_COUNT	7

extern uint32_t _tick;

void MSGEQ7_init();

void MSGEQ7_get(uint32_t*);

void Spectrum_toDb(float*, uint32_t*);


#endif /* MSGEQ7_H_ */

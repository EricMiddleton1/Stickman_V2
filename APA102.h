/*
 * APA102.h
 *
 *  Created on: Sep 11, 2016
 *      Author: eric
 */

#ifndef APA102_H_
#define APA102_H_

#include <stdint.h>

#include "pin_mux_config.h"

typedef struct {
	CS_e cs;
	uint32_t *pixels;
	uint32_t pixelCount;
} APA102_Strip;

void APA102_init();

void APA102_initStrip(APA102_Strip*, CS_e cs, uint32_t pixelCount);

void APA102_clearStrip(APA102_Strip*);

void APA102_setColor(APA102_Strip*, uint32_t pixel, uint8_t r, uint8_t g, uint8_t b);

void APA102_setAll(APA102_Strip*, uint8_t r, uint8_t g, uint8_t b);

void APA102_updateStrip(APA102_Strip*);


#endif /* APA102_H_ */

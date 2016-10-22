/*
 * Matrix.h
 *
 *  Created on: Oct 20, 2016
 *      Author: eric
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <stdint.h>

#include "APA102.h"

typedef struct {
	APA102_Strip strip;
	uint8_t width, height;
} Matrix;

void Matrix_init(Matrix*, CS_e, uint8_t, uint8_t);

void Matrix_clear(Matrix*);

void Matrix_setPixel(Matrix*, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

void Matrix_update(Matrix*);


#endif /* MATRIX_H_ */

/*
 * Matrix.c
 *
 *  Created on: Oct 20, 2016
 *      Author: eric
 */


#include "Matrix.h"

void Matrix_init(Matrix* _matrix, CS_e cs, uint8_t _width, uint8_t _height) {
	APA102_initStrip(&_matrix->strip, cs, _width*_height);

	_matrix->width = _width;
	_matrix->height = _height;
}

void Matrix_clear(Matrix* _matrix) {
	APA102_clearStrip(&_matrix->strip);
}

void Matrix_setPixel(Matrix* _matrix, uint8_t _x, uint8_t _y, uint8_t _r, uint8_t _g, uint8_t _b) {
	uint8_t colOffset = (_x & 0x01) ? (_matrix->height - _y - 1) : _y;

	APA102_setColor(&_matrix->strip, _x*_matrix->height + colOffset, _r, _g, _b);
}

void Matrix_update(Matrix* _matrix) {
	APA102_updateStrip(&_matrix->strip);
}

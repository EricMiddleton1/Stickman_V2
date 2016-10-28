/*
 * Spectrum.h
 *
 *  Created on: Oct 22, 2016
 *      Author: eric
 */

#ifndef SPECTRUM_H_
#define SPECTRUM_H_



#include <stdint.h>

void Spectrum_init(float*);

void Spectrum_fromMSGEQ7(float*, const uint32_t*);


#endif /* SPECTRUM_H_ */

/*
 * Spectrum.h
 *
 *  Created on: Oct 22, 2016
 *      Author: eric
 */

#ifndef SPECTRUM_H_
#define SPECTRUM_H_

#define MAX_VALUE		4096
#define NOISE_FLOOR		-15

#define MAX_DB			22.f

#define ACTIVATE_RATE	0.6f
#define FADE_RATE		0.02f

#define HF_FACTOR		1.8f //Factor to correct for HF roll-off
#define LF_FACTOR		0.8f

#include <stdint.h>

void Spectrum_init(float*);

void Spectrum_fromMSGEQ7(float*, const uint32_t*);


#endif /* SPECTRUM_H_ */

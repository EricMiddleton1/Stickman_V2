/*
 * Spectrum.c
 *
 *  Created on: Oct 22, 2016
 *      Author: eric
 */

#include "Spectrum.h"

#include <math.h>

#include "MSGEQ7.h"

static void __filter(float*, float);

void Spectrum_init(float *spectrum) {
	uint8_t i;
	for(i = 0; i < 5; ++i)
		spectrum[i] = 0.f;
}

void Spectrum_fromMSGEQ7(float *spectrum, const uint32_t *geq) {
	float db[GEQ_CH_COUNT];
	uint8_t i;

	for(i = 0; i < GEQ_CH_COUNT; ++i) {
		db[i] = 20.f * log10((float)geq[i] / MAX_VALUE) - NOISE_FLOOR;
		if(db[i] < 0.f)
			db[i] = 0.f;
	}

	//Filter the new values into the existing spectrum
	__filter(spectrum, db[0]/MAX_DB * LF_FACTOR);
	__filter(spectrum+1, db[1]/MAX_DB);
	__filter(spectrum+2, (db[2] + db[3])/(2.f*MAX_DB));
	__filter(spectrum+3, db[4]/MAX_DB);
	__filter(spectrum+4, (db[5] + db[6])/(2.f*MAX_DB) * HF_FACTOR);
}

static void __filter(float *out, float in) {
	//Limit value to 1.0
	if(in > 1.f)
		in = 1.f;

	if(in >= *out)
		*out = (*out) * (1.f - ACTIVATE_RATE) + in*ACTIVATE_RATE;
	else {
		*out = fmax(*out - FADE_RATE, in);
	}
}


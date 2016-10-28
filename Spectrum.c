/*
 * Spectrum.c
 *
 *  Created on: Oct 22, 2016
 *      Author: eric
 */

#include "Spectrum.h"

#include <math.h>

#include "MSGEQ7.h"

#define MAX_VALUE		16384

#define ACTIVATE_RATE	0.6f
#define FADE_RATE		0.02f

#define HF_FACTOR		1.f //Factor to correct for HF roll-off
#define LF_FACTOR		1.f

static const float NOISE_FLOOR[] = {-20, -18, -15, -15, -18, -16, -10};
static const float DB_MAX[] = {0.75f, 0.75f, 0.5f, 0.5f, 0.4f};

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
		db[i] = 20.f * log10((float)geq[i] / MAX_VALUE) - NOISE_FLOOR[i];
		if(db[i] < 0.f)
			db[i] = 0.f;

		db[i] /= -NOISE_FLOOR[i];
	}

	/*
	char msg[128];
	sprintf(msg, "%3.2f %3.2f %3.2f %3.2f %3.2f %3.2f %3.2f\r\n", db[0], db[1], db[2], db[3], db[4], db[5], db[6]);
	Communicator_sendBuffer(msg, strlen(msg));
	*/

	//Filter the new values into the existing spectrum
	__filter(spectrum, db[0]/ DB_MAX[0]);
	__filter(spectrum+1, db[1] / DB_MAX[1]);
	__filter(spectrum+2, (db[2] + db[3])/DB_MAX[2]);
	__filter(spectrum+3, (db[4])/DB_MAX[3]);
	__filter(spectrum+4, (db[5] + db[6])/DB_MAX[4]);
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


/*
 * BeatDetect.c
 *
 *  Created on: Oct 22, 2016
 *      Author: eric
 */

#include <math.h>

#include "BeatDetect.h"
#include "Spectrum.h"

void BeatDetector_init(BeatDetector* detector, float threshold, float min) {
	detector->lastBeatTime = 0;
	detector->threshold = threshold;
	detector->min = min;
	detector->sPtr = 0;
	detector->sAvg = 0;

	//Clear sample buffer
	uint8_t i;
	for(i = 0; i < SAMPLE_SIZE; ++i) {
		detector->samples[i] = 0.f;
	}
}

bool BeatDetector_detect(BeatDetector* detector, float sample) {
	uint8_t i;

	if(sample < detector->min)
		return false;

	//Calculate average of samples
	float avg = 0;
	for(i = 0; i < SAMPLE_SIZE; ++i) {
		avg += detector->samples[i];
	}
	avg /= SAMPLE_SIZE;

	//Check for beat
	bool beat =	sample >= (avg + detector->threshold);

	//Insert sample into buffer
	detector->samples[detector->sPtr] = sample;
	detector->sPtr = (detector->sPtr + 1) % SAMPLE_SIZE;

	return beat;
}

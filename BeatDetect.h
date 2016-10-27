/*
 * BeatDetect.h
 *
 *  Created on: Oct 23, 2016
 *      Author: eric
 */

#ifndef BEATDETECT_H_
#define BEATDETECT_H_

#include <stdbool.h>
#include <stdint.h>

#define SAMPLE_SIZE		15

typedef struct {
	float samples[SAMPLE_SIZE];
	float sAvg;
	uint8_t sPtr;

	float threshold;
	float min;

	uint32_t lastBeatTime;
} BeatDetector;

void BeatDetector_init(BeatDetector*, float, float);

bool BeatDetector_detect(BeatDetector*, float);

#endif /* BEATDETECT_H_ */

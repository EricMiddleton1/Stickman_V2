/*
 * AudioInput.h
 *
 *  Created on: Sep 14, 2016
 *      Author: eric
 */

#ifndef AUDIOINPUT_H_
#define AUDIOINPUT_H_


#include "adc.h"
#include <stdint.h>

#define AUDIO_BUFFER_SIZE		1024

typedef struct {
	volatile int16_t samples[AUDIO_BUFFER_SIZE];
	volatile uint16_t sampleCount;
} AudioBuffer;

void AudioInput_init();

void AudioInput_getBuffer(AudioBuffer**);





#endif /* AUDIOINPUT_H_ */

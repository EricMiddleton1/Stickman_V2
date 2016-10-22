/*
 * AudioInput.c
 *
 *  Created on: Sep 14, 2016
 *      Author: eric
 */


#include "AudioInput.h"
#include "hw_memmap.h"

#define ADC_CHANNEL				ADC_CH_1
#define BUFFER_COUNT			2

#define CENTER					(1 << 11)

//Audio buffers
AudioBuffer _buffers[BUFFER_COUNT];
uint8_t _curBuffer;
int8_t _completeBuffer;

void adc_interruptHandler();

void AudioInput_init() {
	//Configure buffers
	_buffers[0].sampleCount = 0;
	_curBuffer = 0;
	_completeBuffer = -1;

	//Enable ADC channel 0
	ADCChannelEnable(ADC_BASE, ADC_CHANNEL);

	//Enable the ADC
	ADCEnable(ADC_BASE);

	//Register interrupt function
	ADCIntRegister(ADC_BASE, ADC_CHANNEL, &adc_interruptHandler);

	//Enable fifo full interrupt
	ADCIntEnable(ADC_BASE, ADC_CHANNEL, ADC_FIFO_FULL);
}

void AudioInput_getBuffer(AudioBuffer** bufferOut) {
	//Check for a complete buffer
	if(_completeBuffer != -1) {
		*bufferOut = &_buffers[_completeBuffer];

		_completeBuffer = -1;
	}
	else {
		//Swap buffers
		uint8_t oldBuffer = _curBuffer;
		_curBuffer = (oldBuffer + 1) % BUFFER_COUNT;

		*bufferOut = &_buffers[_completeBuffer];
	}
}

void adc_interruptHandler() {
	//Get interrupt flags
	unsigned long intStatus = ADCIntStatus(ADC_BASE, ADC_CHANNEL);

	//Clear the interrupt flags
	ADCIntClear(ADC_BASE, ADC_CHANNEL, intStatus);

	//Grab pointer to current buffer
	AudioBuffer* curBuff = &_buffers[_curBuffer];

	//Run samples from FIFO through the peak detector
	while(ADCFIFOLvlGet(ADC_BASE, ADC_CHANNEL) > 0) {
		int32_t value = ADCFIFORead(ADC_BASE, ADC_CHANNEL) / 4 - CENTER;

		//Add the sample to the buffer
		curBuff->samples[curBuff->sampleCount++] = value;

		if(curBuff->sampleCount >= AUDIO_BUFFER_SIZE) {
			if(_completeBuffer == -1)
				_completeBuffer = _curBuffer;
			else {
				//This line is just used to set a breakpoint
				volatile int i = 0;
			}

			//Switch buffers
			_curBuffer = (_curBuffer + 1) % BUFFER_COUNT;
		}
	}
}

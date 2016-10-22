/*
 * MSGEQ7.c
 *
 *  Created on: Oct 20, 2016
 *      Author: eric
 */

#include "MSGEQ7.h"

#include <math.h>

#include "hw_memmap.h"
#include "pin_mux_config.h"
#include "timer.h"
#include "gpio.h"
#include "adc.h"

#define TIMER_BASE		TIMERA0_BASE
#define ADC_CHANNEL		ADC_CH_1

#define MAX_VALUE		4096
#define NOISE_FLOOR		-15

#define TIMER_PERIOD	79999	//Gives 100us period

#define ADC_PERIOD_START	40000//5120	//36us into 100us period to account for settle time

static void adc_interruptHandler();
static void timer_handler();

static volatile unsigned int _adcCount;
static volatile uint8_t _state;

static uint32_t _levels[GEQ_CH_COUNT];

void MSGEQ7_init() {
	_adcCount = 0;
	_state = 0;

	uint8_t i;
	for(i = 0; i < GEQ_CH_COUNT; ++i) {
		_levels[i] = 0;
	}

	//Clear reset and strobe pins
	GPIOPinWrite(GEQ_PORT, GEQ_RESET_PIN | GEQ_STROBE_PIN, 0x00);

	//Enable the GPT timer
    MAP_PRCMPeripheralClkEnable(PRCM_TIMERA0, PRCM_RUN_MODE_CLK);
    MAP_PRCMPeripheralReset(PRCM_TIMERA0);

	//Configure timer as periodic full-width
	TimerConfigure(TIMER_BASE, TIMER_CFG_PERIODIC);

	//Set load value
	TimerLoadSet(TIMER_BASE, TIMER_A, TIMER_PERIOD);

	//Register interrupt handler
	TimerIntRegister(TIMER_BASE, TIMER_A, &timer_handler);

	//Enable timeout interrupt
	TimerIntEnable(TIMER_BASE, TIMER_TIMA_TIMEOUT);

	//Enable ADC channel 0
	ADCChannelEnable(ADC_BASE, ADC_CHANNEL);

	//Enable the ADC
	ADCEnable(ADC_BASE);

	//Register interrupt function
	ADCIntRegister(ADC_BASE, ADC_CHANNEL, &adc_interruptHandler);

	//Enable fifo full interrupt
	ADCIntEnable(ADC_BASE, ADC_CHANNEL, ADC_FIFO_FULL);

	//Start the timer
	TimerEnable(TIMER_BASE, TIMER_A);
}

void MSGEQ7_get(uint32_t* out) {
	if(out == 0)
		return;

	uint8_t i;
	for(i = 0; i < GEQ_CH_COUNT; ++i) {
		out[i] = _levels[i];
	}
}

void Spectrum_toDb(float* out, uint32_t* in) {
	if(out == 0 || in == 0)
		return;

	uint8_t i;
	for(i = 0; i < GEQ_CH_COUNT; ++i) {
		out[i] = 20.f*log10((float)in[i] / MAX_VALUE) - NOISE_FLOOR;
		if(out[i] < 0.f)
			out[i] = 0.f;
	}
}

void adc_interruptHandler() {
	//Get interrupt flags
	unsigned long intStatus = ADCIntStatus(ADC_BASE, ADC_CHANNEL);

	//Clear the interrupt flags
	ADCIntClear(ADC_BASE, ADC_CHANNEL, intStatus);

	//Check if GEQ output is valid
	if( !(_state & 0x01) && (TimerValueGet(TIMER_BASE, TIMER_A) < ADC_PERIOD_START) ) {
		//Get current channel
		int8_t channel = (_state >> 1) - 2;
		if(channel < 0)
			channel = GEQ_CH_COUNT - 1;

		//Get all samples from the FIFO
		while(ADCFIFOLvlGet(ADC_BASE, ADC_CHANNEL) > 0) {
			uint32_t value = ADCFIFORead(ADC_BASE, ADC_CHANNEL);

			//Average current value and new value
			_levels[channel] = (_levels[channel] + value) >> 1;
		}
	}
	else {
		//Empty the fifo
		while(ADCFIFOLvlGet(ADC_BASE, ADC_CHANNEL) > 0) {
			volatile uint32_t dummy;
			dummy = ADCFIFORead(ADC_BASE, ADC_CHANNEL);
		}
	}
}

/*
0	0 1
1	2 3
2	4 5
3	6 7
4	8 9
5	10 11
6	12 13
7	14	15
*/

void timer_handler() {
	unsigned long intStatus = TimerIntStatus(TIMER_BASE, 0);
	TimerIntClear(TIMER_BASE, intStatus);

	if(!(_state & 0x01)) {
		//Even numbered state, need to set reset or strobe

		if(_state == 0)
			//clear reset
			GPIOPinWrite(GEQ_PORT, GEQ_RESET_PIN, GEQ_RESET_PIN);
		else
			//Clear strobe
			GPIOPinWrite(GEQ_PORT, GEQ_STROBE_PIN, GEQ_STROBE_PIN);
	}
	else {
		//Odd numbered state, need to set reset or clear
		GPIOPinWrite(GEQ_PORT, GEQ_RESET_PIN | GEQ_STROBE_PIN, 0x00);
	}

	_state = (_state + 1) & 0x0F;

	//Clear adc count
	_adcCount = 0;
}

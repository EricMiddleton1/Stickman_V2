/*
 * BatteryMonitor.c
 *
 *  Created on: Oct 27, 2016
 *      Author: eric
 */


#include "BatteryMonitor.h"

#include <stdlib.h>
#include <stdio.h>
#include "hw_memmap.h"
#include "Communicator.h"
#include "adc.h"

#define MONITOR_STACK_SIZE	4096

#define ADC_CHANNEL		ADC_CH_0

#define CUTOFF_VOLTAGE	(9.f)

#define FILTER_STRENGTH	(0.5f)

#define CONVERSION_FACTOR	(12.51f / 9233.f)

static float _voltage;

static void task_battery(void);

void BatteryMonitor_init() {
	_voltage = -1.f;

	//Enable ADC channel
	ADCChannelEnable(ADC_BASE, ADC_CHANNEL);

	//Enable the ADC
	ADCEnable(ADC_BASE);

	//Launch the monitoring task
	int retval = osi_TaskCreate(task_battery,
			(const signed char*)"Battery Task",
			MONITOR_STACK_SIZE,
			NULL,
			1,
			NULL);
}

float BatteryMonitor_check() {
	return _voltage;
}

void task_battery(void) {
	while(1) {
		uint32_t sum = 0;

		//Grab all samples from the ADC FIFO
		uint8_t count;
		for(count = 0; ADCFIFOLvlGet(ADC_BASE, ADC_CHANNEL) > 0; ++count) {
			sum += ADCFIFORead(ADC_BASE, ADC_CHANNEL);
		}

		//Compute average
		float avg = CONVERSION_FACTOR * ((float)sum / count);

		if(_voltage < 0) {
			//First update
			_voltage = avg;
		}
		else {
			//Filter this average into the stored battery voltage
			_voltage = (_voltage * FILTER_STRENGTH) + (avg * (1.f - FILTER_STRENGTH));
		}
/*
		//FOR DEBUGGING
		char msg[128];
		sprintf(msg, "Voltage: %fv\r\n", _voltage);
		Communicator_sendBuffer(msg, strlen(msg));

*/
		uint16_t voltageInt = _voltage * 1000;

		uint8_t payload[2] = {voltageInt >> 8, (voltageInt & 0xFF)};

		//Send update to app
		Packet p = {
				.type = BATTERY_LEVEL,
				.payload = payload,
				.payloadSize = 2
		};
		Communicator_sendPacket(&p);

		//Wait before checking again
		osi_Sleep(1000);
	}
}

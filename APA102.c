/*
 * APA102.c
 *
 *  Created on: Sep 11, 2016
 *      Author: eric
 */

#include <stdint.h>
#include <math.h>

#include "APA102.h"

#include "hw_types.h"
#include "spi.h"
#include "hw_memmap.h"
#include "prcm.h"

static uint8_t _gamma[256];

#define GAMMA		3.f

#define SPI_CLOCK	8000000 //8Mhz clock

void APA102_init() {
	//Configure SPI module
	SPIConfigSetExpClk(GSPI_BASE,
			PRCMPeripheralClockGet(PRCM_GSPI),
			SPI_CLOCK,
			SPI_MODE_MASTER,
			SPI_SUB_MODE_3,	//Polarity = Phase = 1
			SPI_WL_32 | SPI_3PIN_MODE | SPI_TURBO_OFF);

	//Enable SPI
	SPIEnable(GSPI_BASE);

	//Enable Tx FIFO
	SPIFIFOEnable(GSPI_BASE, SPI_TX_FIFO);

	//Clear all CS pins
	uint16_t i;
	for(i = 0; i < CS_COUNT; ++i) {
		GPIOPinWrite(CS_PORT[i], CS_PIN[i], 0x00);
	}

	//Generate the gamma table
	for(i = 0; i < 256; ++i) {
		_gamma[i] = pow(i / 255.f, GAMMA) * 255.f + 0.5f;
	}
}

void APA102_initStrip(APA102_Strip* strip, CS_e cs, uint32_t pixelCount) {
	//Calculate footer frame count
	int footerCount = 1 + pixelCount / 64;

	//Allocate memory for pixels
	strip->pixels = (uint32_t*)malloc(sizeof(uint32_t) * (1 + pixelCount + footerCount));
	strip->pixelCount = pixelCount;

	int i;

	//Set header
	strip->pixels[0] = 0;

	//Clear pixels/set footers
	for(i = 1; i < (1 + pixelCount + footerCount); i++) {
		strip->pixels[i] = 0xFF000000;	//Set each pixel to off
	}

	//Set cs
	strip->cs = cs;
}

void APA102_clearStrip(APA102_Strip *strip) {
	uint16_t i;
	for(i = 0; i < strip->pixelCount; ++i) {
		strip->pixels[i] &= 0xFF000000;
	}
}

void APA102_setColor(APA102_Strip *strip, uint32_t pixel, uint8_t r, uint8_t g, uint8_t b) {
	if(pixel >= strip->pixelCount)
		return;

	strip->pixels[pixel + 1] = 0xFF000000 | (_gamma[b] << 16) | (_gamma[g] << 8) | _gamma[r];
}

void APA102_setAll(APA102_Strip *strip, uint8_t r, uint8_t g, uint8_t b) {
	uint16_t i;
	for(i = 0; i < strip->pixelCount; ++i) {
		strip->pixels[i+1] = 0xFF000000 | (_gamma[b] << 16) | (_gamma[g] << 8) | _gamma[r];
	}
}

void APA102_updateStrip(APA102_Strip *strip) {
	uint32_t frameCount = 2 + strip->pixelCount + (strip->pixelCount / 64);

	//Set CS pin
	GPIOPinWrite(CS_PORT[strip->cs], CS_PIN[strip->cs], CS_PIN[strip->cs]);

	//Send header, pixels, footers
	SPITransfer(GSPI_BASE, strip->pixels, 0, 4*frameCount, SPI_CS_DISABLE);

	//Clear CS pin
	GPIOPinWrite(CS_PORT[strip->cs], CS_PIN[strip->cs], 0x00);
}

/*
 * Stickman V2
 *
 * Written by Eric Middleton
 */

#define LED_COUNT	94//288//94

#include <stdlib.h>
#include <string.h>

// Simplelink includes
#include "simplelink.h"

// driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "gpio.h"
#include "Console.h"

// free_rtos/ti-rtos includes
#include "osi.h"

// common interface includes
#include "common.h"
#ifndef NOTERM
#include "uart_if.h"
#endif

#define APP_NAME                "CPRE288 UART->WiFi adapter"
#define APPLICATION_VERSION     "1.1.1"
#define OSI_STACK_SIZE          2048

#define DEFAULT_BAUD			115200

#define CHEST_WIDTH				5
#define CHEST_HEIGHT			20

#define ARM_LENGTH				27
#define LEG_LENGTH				38

#include "pin_mux_config.h"

//Custom tasks
#include "APA102.h"
#include "Matrix.h"
#include "MSGEQ7.h"
#include "Spectrum.h"

#include <math.h>

//*****************************************************************************
//
//! Board Initialization & Configuration
//! Note: Taken from wlan_ap example program
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
    //
    // Set vector table base
    //
#if defined(ccs) || defined(gcc)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

void displayLevel(uint8_t _level) {
	if(_level > 80)
		_level = 80;

	char display[81], *dPtr = display;

	while((_level--) > 0)
		*(dPtr++) = '*';
	*dPtr = '\0';

	Console_puts(display);
}

/*
 *  ======== main ========
 */
int main(void)
{
	//Board initialization
	BoardInit();

    //Initialize the pin configuration
    PinMuxConfig();

    //Initialize LEDs
    GPIODirModeSet(LED_PORT, LED_RED | LED_ORANGE | LED_GREEN,
    		GPIO_DIR_MODE_OUT);
    //Clear the LEDs
    GPIOPinWrite(LED_PORT, LED_RED | LED_ORANGE | LED_GREEN, 0x00);

    //Set the red LED
    GPIOPinWrite(LED_PORT, LED_RED, 0xFF);


    //Initialize the APA102 driver
    APA102_init();

    //Initialize the MSGEQ7 driver
    MSGEQ7_init();

    //Initialize the chest matrix
    Matrix matrix;
    Matrix_init(&matrix, CS_CHEST, CHEST_WIDTH, CHEST_HEIGHT);

    //Initialize the appendage strips
    APA102_Strip armLeft, armRight, legLeft, legRight;
    APA102_initStrip(&armLeft, CS_ARM_L, ARM_LENGTH);
    APA102_initStrip(&armRight, CS_ARM_R, ARM_LENGTH);
    APA102_initStrip(&legLeft, CS_LEG_L, LEG_LENGTH);
    APA102_initStrip(&legRight, CS_LEG_R, LEG_LENGTH);

    Matrix_update(&matrix);

    //Set the orange LED
    GPIOPinWrite(LED_PORT, LED_ORANGE, 0xFF);

	uint32_t geq[GEQ_CH_COUNT];
	float spectrum[5];

	Spectrum_init(spectrum);

    while(1) {
    	static uint16_t i = 0;
    	static uint8_t color = 0;
    	static uint16_t brt = 192;
    	static int8_t dir = 1;

    	//Get the current spectrum
    	MSGEQ7_get(geq);

    	Spectrum_fromMSGEQ7(spectrum, geq);

    	Matrix_clear(&matrix);

    	uint8_t x;
    	uint8_t reds[] = {brt, brt, 0, 0, 0, brt};
    	uint8_t greens[] = {0, brt, brt, brt, 0, 0};
    	uint8_t blues[] = {0, 0, 0, brt, brt, brt};

    	for(x = 0; x < 5; ++x) {
    		uint8_t level = CHEST_HEIGHT * spectrum[x], y;
    		if(level < 1)
    			level = 1;

    		for(y = 0; y < level; ++y) {
    			Matrix_setPixel(&matrix, x, y, reds[x], greens[x], blues[x]);
    		}
    	}

    	APA102_setAll(&armLeft, reds[color], greens[color], blues[color]);
    	uint8_t c = (color+1) % 6;
    	APA102_setAll(&armRight, reds[c], greens[c], blues[c]);

    	Matrix_update(&matrix);
    	APA102_updateStrip(&armLeft);
    	APA102_updateStrip(&armRight);
    	APA102_updateStrip(&legLeft);
    	APA102_updateStrip(&legRight);

    	/*
    	brt += dir;
    	if(brt >= 256) {
    		dir *= -1;
    		brt += dir;

    		if(brt == 0)
    			color = (color + 2) % 6;
    	}*/
    	++i;
    	if(i == 100) {
    		i = 0;
    		color = (color + 1) % 6;
    	}

    	UtilsDelay(80000/3); //Delay 1ms
    }

    int retval;

    //Start simplelink host
    retval = VStartSimpleLinkSpawnTask(SPAWN_TASK_PRIORITY);
    if(retval < 0) {
    	for(;;);
    }

    if(retval < 0) {
    	for(;;);
    }

    //Start task scheduler
    osi_start();

    return 0;
}

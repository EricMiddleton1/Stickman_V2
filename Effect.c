/*
 * Effect.c
 *
 *  Created on: Oct 23, 2016
 *      Author: eric
 */

#include "Effect.h"

#include <stdbool.h>
#include <stdlib.h>

#include "osi.h"

#include "APA102.h"
#include "Matrix.h"
#include "MSGEQ7.h"
#include "Spectrum.h"
#include "BeatDetect.h"

#define STACK_SIZE		4096

#define CHEST_WIDTH				5
#define CHEST_HEIGHT			20

#define ARM_LENGTH				27
#define LEG_LENGTH				38

#define DEFAULT_EFFECT	SOUND_REACTIVE

struct Effect {
	void (*start)(void**);
	void (*update)(void**, uint32_t);
	void (*end)(void**);

	void (*dataReceive)(void**, uint8_t*, uint32_t);

	void **effectData;
};

static void solidStickStart(void**);
static void solidStickUpdate(void**, uint32_t);
static void solidStickEnd(void**);
static void solidStickData(void**, uint8_t*, uint32_t);

static void pulseStickStart(void**);
static void pulseStickUpdate(void**, uint32_t);
static void pulseStickEnd(void**);
static void pulseStickData(void**, uint8_t*, uint32_t);

static void chestMsgStart(void**);
static void chestMsgUpdate(void**, uint32_t);
static void chestMsgEnd(void**);
static void chestMsgData(void**, uint8_t*, uint32_t);

static void soundStart(void**);
static void soundUpdate(void**, uint32_t);
static void soundEnd(void**);
static void soundData(void**, uint8_t*, uint32_t);

void *_effectData[MAX_EFFECT];

static const struct Effect _effects[] = {
		{&solidStickStart, &solidStickUpdate, &solidStickEnd, &solidStickData, &_effectData[0]},
		{&pulseStickStart, &pulseStickUpdate, &pulseStickEnd, &pulseStickData, &_effectData[1]},
		{&chestMsgStart, &chestMsgUpdate, &chestMsgEnd, &chestMsgData, &_effectData[2]},
		{&soundStart, &soundUpdate, &soundEnd, &soundData, &_effectData[3]}
};

//LED strips
static Matrix matrix;
static APA102_Strip armLeft, armRight, legLeft, legRight;

static void __effectTask(void);

void Effect_start() {
    //Initialize the APA102 driver
    APA102_init();

    //Initialize the chest matrix
    Matrix_init(&matrix, CS_CHEST, CHEST_WIDTH, CHEST_HEIGHT);

    //Initialize the appendage strips
    APA102_initStrip(&armLeft, CS_ARM_L, ARM_LENGTH);
    APA102_initStrip(&armRight, CS_ARM_R, ARM_LENGTH);
    APA102_initStrip(&legLeft, CS_LEG_L, LEG_LENGTH);
    APA102_initStrip(&legRight, CS_LEG_R, LEG_LENGTH);

    //Launch Effect update task
    osi_TaskCreate(__effectTask,
    		(const signed char*)"Effect Task",
			STACK_SIZE,
			NULL,
			1,
			NULL);
}

void __effectTask(void) {
	Effect_e currentEffect = DEFAULT_EFFECT;

	bool newEffect = true;

	while(1) {
		const struct Effect *ePtr = &_effects[(uint8_t)currentEffect];

		if(newEffect) {
			//Call the effect's start routine
			if(ePtr->start != NULL)
				ePtr->start(ePtr->effectData);

			newEffect = false;
		}

		//Update the effect
		ePtr->update(ePtr->effectData, _tick);

		//TODO: Check for messages from WiFi system

		//TODO: make this delay take processing time into account
		osi_Sleep(10);
	}
}


static void solidStickStart(void** settings) {

}

static void solidStickUpdate(void** settings, uint32_t time) {

}

static void solidStickEnd(void** settings) {

}

static void solidStickData(void** settings, uint8_t* data, uint32_t length) {

}


static void pulseStickStart(void** settings) {

}

static void pulseStickUpdate(void** settings, uint32_t time) {

}

static void pulseStickEnd(void** settings) {

}

static void pulseStickData(void** settings, uint8_t* data, uint32_t length) {

}


static void chestMsgStart(void** settings) {

}

static void chestMsgUpdate(void** settings, uint32_t time) {

}

static void chestMsgEnd(void** settings) {

}

static void chestMsgData(void** settings, uint8_t* data, uint32_t length) {

}

typedef struct {
	uint8_t brightness;
} SoundSettings;

static void soundStart(void** settings) {
    //Initialize the MSGEQ7 driver
    MSGEQ7_init();

    *settings = malloc(sizeof(SoundSettings));

    ((SoundSettings*)(*settings))->brightness = 192;
}

static void soundUpdate(void** settings, uint32_t time) {
	const static uint8_t reds[] = {1, 1, 0, 0, 0, 1};
	const static uint8_t greens[] = {0, 1, 1, 1, 0, 0};
	const static uint8_t blues[] = {0, 0, 0, 1, 1, 1};

	static uint8_t first = 1;
	static float spectrum[5];
	static BeatDetector bassBeat, trebbleBeat;
	static uint8_t bassHit = 0, trebHit = 0;

	if(first) {
		//Initial setup of static variables

		Spectrum_init(spectrum);

		BeatDetector_init(&bassBeat, 0.2f, 0.05f);
		BeatDetector_init(&trebbleBeat, 0.2f, 0.05f);

		first = 0;
	}

	uint32_t geq[GEQ_CH_COUNT];

	//Grab current brightness
	uint8_t brightness = ((SoundSettings*)*settings)->brightness;

	//Get the current spectrum
	MSGEQ7_get(geq);

	//Convert into spectrum representation
	Spectrum_fromMSGEQ7(spectrum, geq);

	//Check for beats
	if(BeatDetector_detect(&bassBeat, spectrum[0])) {
		bassHit = brightness;
	}

	if(BeatDetector_detect(&trebbleBeat, spectrum[4])) {
		trebHit = brightness;
	}


	Matrix_clear(&matrix);

	//Update matrix with spectrum
	uint8_t x;
	for(x = 0; x < 5; ++x) {
		uint8_t level = CHEST_HEIGHT * spectrum[x], y;
		if(level < 1)
			level = 1;

		for(y = 0; y < level; ++y) {
			Matrix_setPixel(&matrix, x, y, brightness*reds[x], brightness*greens[x], brightness*blues[x]);
		}
	}

	//Update appendages with 'hits'
	APA102_setAll(&armLeft, 0, 0, trebHit);
	//APA102_setAll(&armLeft, bassHit, 0, 0);
	APA102_setAll(&armRight, 0, 0, trebHit);
	APA102_setAll(&legLeft, bassHit, 0, 0);
	APA102_setAll(&legRight, bassHit, 0, 0);

	//Flush updates to strips
	Matrix_update(&matrix);
	APA102_updateStrip(&armLeft);
	APA102_updateStrip(&armRight);
	APA102_updateStrip(&legLeft);
	APA102_updateStrip(&legRight);

	//Update bass/trebble hits
	if(trebHit > 0) {
		if(trebHit > 10)
			trebHit -= 10;
		else
			trebHit = 0;
	}
	if(bassHit > 0) {
		if(bassHit > 10)
			bassHit -= 10;
		else
			bassHit = 0;
	}
}

static void soundEnd(void** settings) {
	free((SoundSettings*)*settings);
}

static void soundData(void** settings, uint8_t* data, uint32_t length) {

}

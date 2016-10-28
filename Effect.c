/*
 * Effect.c
 *
 *  Created on: Oct 23, 2016
 *      Author: eric
 */

#include "Effect.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "osi.h"

#include "APA102.h"
#include "Matrix.h"
#include "MSGEQ7.h"
#include "Spectrum.h"
#include "BeatDetect.h"
#include "Communicator.h"
#include "Color.h"

#define STACK_SIZE		4096

#define CHEST_WIDTH				5
#define CHEST_HEIGHT			20

#define ARM_LENGTH				27
#define LEG_LENGTH				38

#define DEFAULT_EFFECT	SOLID_STICKMAN

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

static const char* _effectNames[] = {
		"Solid Stickman",
		"Pulsing Stickman",
		"Chest Message",
		"Sound Reactive"
};

//LED strips
static Matrix matrix;
static APA102_Strip armLeft, armRight, legLeft, legRight;

static Effect_e _globalEffect;

static void __effectTask(void);

static void cbPacketListModeQuery(const Packet*);
static void cbPacketModeSet(const Packet*);
static void cbPacketListParams(const Packet*);

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

    //Register handlers for packets
    Communicator_registerHandler(LIST_MODE_QUERY, &cbPacketListModeQuery);
    Communicator_registerHandler(MODE_SET, &cbPacketModeSet);

    _globalEffect = DEFAULT_EFFECT;

    //Launch Effect update task
    osi_TaskCreate(__effectTask,
    		(const signed char*)"Effect Task",
			STACK_SIZE,
			NULL,
			1,
			NULL);
}

void __effectTask(void) {
	Effect_e currentEffect = MAX_EFFECT;

	while(1) {
		if(currentEffect != _globalEffect) {
			if(currentEffect != MAX_EFFECT) {
				//Call end routine
				_effects[(uint8_t)currentEffect].end(_effects[(uint8_t)currentEffect].effectData);
			}

			//Select new effect
			currentEffect = _globalEffect;

			//Call new effect start routine
			_effects[(uint8_t)currentEffect].start(_effects[(uint8_t)currentEffect].effectData);
		}

		const struct Effect *ePtr = &_effects[(uint8_t)currentEffect];

		//Update the effect
		ePtr->update(ePtr->effectData, _tick);

		//TODO: make this delay take processing time into account
		osi_Sleep(10);
	}
}

void cbPacketListModeQuery(const Packet* _in) {
	uint16_t length = 0;
	uint8_t buffer[128];

	uint8_t i;
	for(i = 0; i < MAX_EFFECT; ++i) {
		length += 1 + strlen(_effectNames[i]);
	}

	Packet p;

	p.type = (uint8_t)LIST_MODE_RESPONSE;
	p.payloadSize = length + 1;

	//p.payload = (uint8_t*)malloc(length + 1);
	p.payload = buffer;

	p.payload[0] = MAX_EFFECT;

	for(i = 0; i < MAX_EFFECT; ++i) {
		static uint16_t index = 1;

		uint8_t cpySize = strlen(_effectNames[i]) + 1;
		strcpy(p.payload + index, _effectNames[i]);
		//memcpy(p.payload + index, _effectNames[i], cpySize);

		index += cpySize;
	}

	//Send packet
	Communicator_sendPacket(&p);
}
void cbPacketModeSet(const Packet* _in) {
	if(_in->payloadSize != 1) {
		//Error
		return;
	}

	if(_in->payload[0] < MAX_EFFECT) {
		//Set new effect
		_globalEffect = _in->payload[0];
	}
}

typedef struct SolidSettings {
	Color colors[3];
	uint8_t brightness;
} SolidSettings;

static void solidStickStart(void** settings) {
	static SolidSettings solidSettings = {
			{ {1.f, 1.f, 0.f}, {1.f, 1.f, 0.f}, {1.f, 0.f, 0.f} },
			192
	};

	*settings = &solidSettings;
}

static void solidStickUpdate(void** settings, uint32_t time) {
	SolidSettings *solidSettings = *settings;
	uint8_t brt = solidSettings->brightness;


	Matrix_clear(&matrix);

	uint8_t r = brt * (solidSettings->colors[0].r), g = brt * (solidSettings->colors[0].g), b = brt * (solidSettings->colors[0].b);
	uint8_t y;
	for(y = 0; y < matrix.height; ++y) {
		Matrix_setPixel(&matrix, 2, y, r, g, b);
	}

	APA102_setAll(&armLeft, brt * (solidSettings->colors[1].r), brt * (solidSettings->colors[1].g), brt * (solidSettings->colors[1].b));
	APA102_setAll(&armRight, brt * (solidSettings->colors[1].r), brt * (solidSettings->colors[1].g), brt * (solidSettings->colors[1].b));
	APA102_setAll(&legLeft, brt * (solidSettings->colors[2].r), brt * (solidSettings->colors[2].g), brt * (solidSettings->colors[2].b));
	APA102_setAll(&legRight, brt * (solidSettings->colors[2].r), brt * (solidSettings->colors[2].g), brt * (solidSettings->colors[2].b));

	Matrix_update(&matrix);
	APA102_updateStrip(&armLeft);
	APA102_updateStrip(&armRight);
	APA102_updateStrip(&legLeft);
	APA102_updateStrip(&legRight);
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
	//APA102_setAll(&armLeft, 0, 0, trebHit);
	//APA102_setAll(&armRight, 0, 0, trebHit);
	//APA102_setAll(&legLeft, bassHit, 0, 0);
	//APA102_setAll(&legRight, bassHit, 0, 0);
	APA102_setAll(&armLeft, 255, 255, 0);
	APA102_setAll(&armRight, 255, 255, 0);
	APA102_setAll(&legLeft, 255, 0, 0);
	APA102_setAll(&legRight, 255, 0, 0);

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

/*
 * Effect.h
 *
 *  Created on: Oct 23, 2016
 *      Author: eric
 */

#ifndef EFFECT_H_
#define EFFECT_H_

#include <stdint.h>


typedef enum {
	SOLID_STICKMAN = 0,
	PULSING_STICKMAN,
	CHEST_MESSAGE,
	SOUND_REACTIVE,


	MAX_EFFECT
} Effect_e;


void Effect_start();


#endif /* EFFECT_H_ */

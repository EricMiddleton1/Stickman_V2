/*
 * Communicator.h
 *
 *  Created on: Oct 27, 2016
 *      Author: eric
 */

#ifndef COMMUNICATOR_H_
#define COMMUNICATOR_H_

#include <stdint.h>

typedef enum PacketType {
	LIST_MODE_QUERY = 0,
	LIST_MODE_RESPONSE,
	MODE_SET,
	MODE_PARAM_SET,
	MODE_PARAM_GET,
	BATTERY_LEVEL,

	MAX_TYPE
} PacketType;

typedef struct Packet {
	PacketType type;

	uint8_t *payload;
	uint16_t payloadSize;
} Packet;

typedef void(*PacketHandler)(const Packet*);


void Communicator_init();

void Communicator_processBuffer(const uint8_t*, uint32_t);

void Communicator_sendPacket(const Packet*);

void Communicator_registerHandler(PacketType, PacketHandler);

#endif /* COMMUNICATOR_H_ */

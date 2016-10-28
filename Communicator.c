/*
 * Communicator.c
 *
 *  Created on: Oct 27, 2016
 *      Author: eric
 */


#include <stdlib.h>
#include "Communicator.h"
#include "task_wifi.h"

#define HEADER_VALUE		0xAF

static PacketHandler _handlers[(uint8_t)MAX_TYPE];


static void _dispatchPacket(const Packet*);

void Communicator_init() {
	uint8_t i;
	for(i = 0; i < (uint8_t)MAX_TYPE; ++i) {
		_handlers[i] = NULL;
	}
}

void Communicator_processBuffer(const uint8_t* buffer, uint32_t size) {
	static Packet tempPacket = {MAX_TYPE, NULL, 0};

	static enum {
		HDR,
		TYPE,
		SIZE_H,
		SIZE_L,
		PAYLOAD
	}  state;
	static uint16_t tempSize;

	uint32_t i;
	for(i = 0; i < size; ++i) {
		uint8_t byte = buffer[i];

		switch(state) {
		case HDR:
			if(byte == HEADER_VALUE)
				state = TYPE;
		break;

		case TYPE:
			if(byte < MAX_TYPE) {
				tempPacket.type = byte;
				state = SIZE_H;
			}
			else {
				//Invalid type
				state = HEADER;
			}
		break;

		case SIZE_H:
			tempPacket.payloadSize = (byte) << 8;

			state = SIZE_L;
		break;

		case SIZE_L:
			tempPacket.payloadSize |= byte;
			tempSize = 0;

			if(tempPacket.payloadSize > 0) {
				//Allocate payload memory
				tempPacket.payload = (uint8_t*)malloc(tempPacket.payloadSize);

				state = PAYLOAD;
			}
			else {
				tempPacket.payload = NULL;

				//Packet is done!
				_dispatchPacket(&tempPacket);

				state = HEADER;
			}
		break;

		case PAYLOAD:
			tempPacket.payload[tempSize++] = byte;

			if(tempSize >= tempPacket.payloadSize) {
				//Packet is done!
				_dispatchPacket(&tempPacket);

				//Clean up memory
				free(tempPacket.payload);

				state = HEADER;
			}
		break;

		default:
			state = HEADER;
		}
	}
}

void Communicator_registerHandler(PacketType type, PacketHandler handler) {
	_handlers[(uint8_t)type] = handler;
}

void Communicator_sendPacket(const Packet* packet) {
	//Allocate memory for packet
	static uint8_t buffer[128];

	//Fill header and size fields
	buffer[0] = HEADER_VALUE;
	buffer[1] = (uint8_t)packet->type;
	buffer[2] = packet->payloadSize >> 8;
	buffer[3] = (packet->payloadSize) & 0xFF;

	//Copy the payload
	memcpy(buffer + 4, packet->payload, packet->payloadSize);

	//Send the memory
	wifi_send(buffer, 4 + packet->payloadSize);
}

static void _dispatchPacket(const Packet* packet) {
	uint8_t handlerIndex = (uint8_t)packet->type;

	if(_handlers[handlerIndex] != NULL) {
		_handlers[handlerIndex](packet);
	}
}

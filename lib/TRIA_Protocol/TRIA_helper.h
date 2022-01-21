#pragma once

#include <fields/TRIA_ID.h>

bool packet_ok(uint8_t* nw_bytes, uint8_t received_length, TRIA_ID& receiver_id);
#pragma once

#include <fields/TRIA_ID.h>
#include <packets/TRIA_GenericPacket.h>

bool deserialise_packet(uint8_t *nw_bytes, uint8_t received_length, TRIA_ID &receiver_id,
                        TRIA_GenericPacket **out);
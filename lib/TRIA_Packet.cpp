#include <lib/TRIA_Packet.h>

bool TRIA_Packet::is_addressed_to(TRIA_id id) {
  return id.matches_mask(this->receiver_id);
}
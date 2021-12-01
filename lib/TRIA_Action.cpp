#include <lib/TRIA_Action.h>

action TRIA_Action::value() { return m_action; };

uint8_t* TRIA_Action::packed() {
  return (uint8_t*)(&this->m_action);
}

size_t TRIA_Action::packed_size() {
  return sizeof(this->m_action);
}
#pragma once

#include <fields/TRIA_Field.h>

enum action {
  ping = 0,
  data_pong = 1,
  measure_report = 2,
};

class TRIA_Action : public TRIA_Field {
public:
  static const size_t PACKED_SIZE = sizeof(uint8_t);

  TRIA_Action() : m_action(ping) {}
  TRIA_Action(action a) : m_action(a) {};
  action value();

  size_t pack_into(uint8_t *buffer) override;
  size_t packed_size() override;
  void initialise_from_buffer(uint8_t *buffer) override;
  void print() override;

private:
  action m_action;
};
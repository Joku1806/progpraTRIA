#pragma once

#include <fields/TRIA_Action.h>
#include <fields/TRIA_Field.h>
#include <fields/TRIA_ID.h>
#include <fields/TRIA_Stamp.h>
#include <stddef.h>
#include <stdint.h>

enum field_positions {
  action_position = 0,
  sender_id_position = 1,
  receiver_id_position = 2,
  timediff_D_position = 3,
  timediff_R_position = 4,
};

class TRIA_GenericPacket : public TRIA_Field {
public:
  // gehe von größtem Paket aus
  // FIXME: muss immer aufpassen, dass die maximalen Messungen nicht erhöht werden, ist ein Problem,
  // sollte wahrscheinlich eine globale config oder so geben
  // vielleicht sollte GenericPacket auch einfach diese Variablen nicht haben, weil die Bedeutung
  // hier ander ist als in den spezialisierten Klassen.
  static const size_t PACKED_SIZE = 30;
  static const size_t FIELD_COUNT = 6;

  bool is_type(action a);
  bool is_addressed_to(TRIA_ID &id);
  TRIA_ID sent_to() { return *(TRIA_ID *)m_fields[receiver_id_position]; }
  TRIA_ID received_from() { return *(TRIA_ID *)m_fields[sender_id_position]; }

  size_t pack_into(uint8_t *bytes) override;
  size_t packed_size() override;
  void initialise_from_buffer(uint8_t *buffer) override;
  void print() override;

protected:
  TRIA_Field *m_fields[FIELD_COUNT];

  const char *m_field_labels[FIELD_COUNT] = {"Unknown", "Unknown", "Unknown",
                                             "Unknown", "Unknown", "Unknown"};
  virtual const char **field_labels() { return m_field_labels; };

  virtual void overwrite_fields() = 0;
  virtual size_t field_count() = 0;
};
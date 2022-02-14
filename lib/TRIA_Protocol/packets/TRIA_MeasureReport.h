#pragma once

#include <Ringbuffer.h>
#include <packets/TRIA_GenericPacket.h>
#include <packets/TRIA_Measure.h>

class TRIA_MeasureReport : public TRIA_GenericPacket {
public:
  static const size_t MAX_MEASURES = 3;
  static const size_t PACKED_SIZE = TRIA_Action::PACKED_SIZE + 2 * TRIA_ID::PACKED_SIZE +
                                    MAX_MEASURES * TRIA_Measure::PACKED_SIZE;
  // FIXME: bin mir nicht sicher ob das hier richtig ist, gibt maximale Anzahl von Feldern an
  static const size_t FIELD_COUNT = 3 + MAX_MEASURES;

  TRIA_MeasureReport()
    : m_action(TRIA_Action(measure_report)), m_sender_id(TRIA_ID()), m_receiver_id(TRIA_ID()) {
    overwrite_fields();
  };

  TRIA_MeasureReport(const TRIA_ID &sid, const TRIA_ID &rid)
    : m_action(TRIA_Action(measure_report)), m_sender_id(sid), m_receiver_id(rid) {
    overwrite_fields();
  };

  // FIXME: rausfinden, warum das nicht const sein kann, bin mir jedenfalls nicht sicher ob das
  // überhaupt wichtig ist
  TRIA_MeasureReport(const TRIA_MeasureReport &other)
    : m_action(other.m_action), m_sender_id(other.m_sender_id), m_receiver_id(other.m_receiver_id) {
    for (size_t i = 0; i < other.m_measures.size(); i++) {
      TRIA_Measure fragment = other.m_measures.at(i);
      m_measures.push_back(fragment);
    }

    overwrite_fields();
  }

  TRIA_MeasureReport &operator=(const TRIA_MeasureReport &other) {
    m_action = other.m_action;
    m_sender_id = other.m_sender_id;
    m_receiver_id = other.m_receiver_id;

    // FIXME: geht auch mit operator=, bin mir gerade auch nicht ganz sicher ob das ein deepcopy ist
    for (size_t i = 0; i < other.m_measures.size(); i++) {
      TRIA_Measure fragment = other.m_measures.at(i);
      m_measures.push_back(fragment);
    }

    overwrite_fields();
    return *this;
  }

  ~TRIA_MeasureReport() {}

  void add_measurement(TRIA_Measure &measure) {
    m_measures.push_back(measure);
    m_fields[3 + m_measures.size() - 1] = m_measures.at_ptr(m_measures.size() - 1);
  }

  bool finished() { return m_measures.is_full(); }

  void reset() { m_measures.clear(); }

  size_t entries() { return m_measures.size(); }

private:
  TRIA_Action m_action;
  TRIA_ID m_sender_id;
  TRIA_ID m_receiver_id;
  Ringbuffer<TRIA_Measure, MAX_MEASURES> m_measures;

  // FIXME: initialise_from_buffer() muss auch überschrieben werden
  void overwrite_fields() override {
    m_fields[action_position] = (TRIA_Field *)&m_action;
    m_fields[sender_id_position] = (TRIA_Field *)&m_sender_id;
    m_fields[receiver_id_position] = (TRIA_Field *)&m_receiver_id;

    for (size_t i = 0; i < m_measures.size(); i++) {
      m_fields[3 + i] = (TRIA_Field *)m_measures.at_ptr(i);
    }
  }

  void initialise_from_buffer(uint8_t *buffer) override {
    for (size_t i = 0; i < 3; i++) {
      m_fields[i]->initialise_from_buffer(buffer);
      buffer += m_fields[i]->packed_size();
    }

    reset();
    TRIA_Measure measure;
    // FIXME: nur zum Testen, es sollte ein Feld im Paket geben, wo die Anzahl an Messungen
    // drinsteht, sollte außerdem wahrscheinlich zur besseren Unterteilung in eine cpp Datei
    for (size_t i = 0; i < MAX_MEASURES; i++) {
      measure.initialise_from_buffer(buffer);
      buffer += measure.packed_size();
      add_measurement(measure);
    }
  }

  size_t field_count() override { return 3 + m_measures.size(); }
};
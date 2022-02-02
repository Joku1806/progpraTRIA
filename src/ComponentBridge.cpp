
#include <TRIA_helper.h>
#include <packets/TRIA_RangeReport.h>
#include <packets/TRIA_RangeRequest.h>
#include <packets/TRIA_RangeResponse.h>
#include <src/ComponentBridge.h>

void ComponentBridge::initialise(
    void (*dw_receive_interrupt_handler)(const dwt_cb_data_t *cb_data)) {
  // LoRa Chipselect auf HIGH schalten, damit er nicht während der SPI Kommunikation mit dem
  // DW3000 stört.
  pinMode(LoRa_chipselect, OUTPUT);
  digitalWrite(LoRa_chipselect, HIGH);
  pinMode(SPI_chipselect, OUTPUT);
  SPI.begin();

#if defined(DEBUG) || defined(COORDINATOR)
  Serial.begin(9600);
  while (!Serial) {};
#endif

  m_id = build_id();
  m_dw_interface = DW3000_Interface(m_id, dw_receive_interrupt_handler);
  VERIFY(m_rf95.init());
}

TRIA_ID ComponentBridge::build_id() {
  uint8_t type = 0;

#ifdef TRACKER
  type |= tracker;
#endif

#ifdef TRACKEE
  type |= trackee;
#endif

#ifdef COORDINATOR
  type |= coordinator;
#endif

#ifdef UNIT_ID
  return TRIA_ID(static_cast<TRIA_dev_type>(type), UNIT_ID);
#else
  return TRIA_ID(static_cast<TRIA_dev_type>(type), 1);
#endif
}

bool ComponentBridge::measurement_requested() {
  return m_id.is_coordinator() && m_usb_interface.measurement_requested();
}

void ComponentBridge::start_measurement() {
  TRIA_RangeRequest request = TRIA_RangeRequest(m_id, TRIA_ID(trackee));
  send_packet_over_lora(request);
}

// FIXME: besserer Name
bool ComponentBridge::did_receive_lora_messages() { return m_rf95.available(); }

void ComponentBridge::handle_received_lora_messages() {
  while (m_rf95.available()) {
    uint8_t recv_length = sizeof(m_recv_buffer);
    if (!m_rf95.recv(m_recv_buffer, &recv_length)) {
      return;
    }

    TRIA_GenericPacket *received = nullptr;
    if (!deserialise_packet(m_recv_buffer, recv_length, m_id, &received)) {
      continue;
    }
    VERIFY(received != nullptr);

#ifdef DEBUG
    Serial.print("Paket empfangen (LoRa): ");
    received->print();
    Serial.print("\n");
#endif

    if (received->is_type(range_request) && received->received_from().is_coordinator()) {
      TRIA_RangeRequest repeated = TRIA_RangeRequest(m_id, TRIA_ID(tracker));
      m_dw_interface.send_range_request(repeated);
    }

    if (received->is_type(range_report) && m_id.is_coordinator() &&
        !m_usb_interface.schedule_full()) {
      // FIXME: Irgendwie ohne cast hinkriegen
      m_usb_interface.schedule_report(*static_cast<TRIA_RangeReport *>(received));
    }
  }
}

// FIXME: besserer Name
bool ComponentBridge::did_receive_dw_messages() {
  return m_dw_interface.unprocessed_messages_pending();
}

void ComponentBridge::handle_received_dw_messages() {
  while (m_dw_interface.unprocessed_messages_pending()) {
    BinaryMessage message = m_dw_interface.get_first_unprocessed_message();

    TRIA_GenericPacket *received = nullptr;
    if (!deserialise_packet(message.data, message.data_length, m_id, &received)) {
      continue;
    }
    VERIFY(received != nullptr);

#ifdef DEBUG
    Serial.println("Paket empfangen (DW): ");
    received->print();
    Serial.print("\n");
#endif

    if (received->is_type(range_request)) {
      auto response =
          TRIA_RangeResponse(m_id, received->received_from(), TRIA_Stamp(message.receive_time));
      m_dw_interface.send_range_response(response);
    } else if (received->is_type(range_response)) {
      TRIA_Stamp message_rx = TRIA_Stamp(message.receive_time);
      TRIA_Stamp original_tx = m_dw_interface.get_tx_stamp();
      TRIA_Stamp measured_rx = static_cast<TRIA_RangeResponse *>(received)->get_rx_stamp();
      TRIA_Stamp measured_tx = static_cast<TRIA_RangeResponse *>(received)->get_tx_stamp();
      // FIXME: vielleicht sollten wir die Ungenauigkeitschecks schon hier machen, anstatt auf
      // der Data Team Seite
      message_rx = message_rx - (measured_tx - measured_rx);

      auto report = TRIA_RangeReport(received->received_from(), m_id, message_rx, original_tx);
      send_packet_over_lora(report);
    }
  }
}

bool ComponentBridge::last_measurement_finished() {
  return m_id.is_coordinator() && m_usb_interface.schedule_is_reset();
}

bool ComponentBridge::current_measurement_finished() {
  return m_id.is_coordinator() &&
         (m_usb_interface.schedule_full() || m_usb_interface.schedule_likely_finished());
}

void ComponentBridge::send_current_measurement() {
  m_usb_interface.send_scheduled_reports();
  m_usb_interface.schedule_reset();
}

void ComponentBridge::send_packet_over_lora(TRIA_GenericPacket &packet) {
#ifdef DEBUG
  Serial.print("Paket gesendet (LoRa): ");
  packet.print();
  Serial.print("\n");
#endif

  packet.pack_into(m_send_buffer);
  VERIFY(m_rf95.send(m_send_buffer, packet.packed_size()));
}

void ComponentBridge::receive_dw_message(const dwt_cb_data_t *cb_data) {
  m_dw_interface.store_received_message(cb_data);
  dwt_writefastCMD(CMD_RX);
}
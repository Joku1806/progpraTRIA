
#include <TRIA_helper.h>
#include <lib/assertions.h>
#include <packets/TRIA_MeasureReport.h>
#include <packets/TRIA_RangeRequest.h>
#include <packets/TRIA_RangeResponse.h>
#include <platform/pin_mappings.h>
#include <src/ComponentBridge.h>

void ComponentBridge::initialise(
    void (*dw_receive_interrupt_handler)(const dwt_cb_data_t *cb_data)) {
  // LoRa Chipselect auf HIGH schalten, damit er nicht während der SPI Kommunikation mit dem
  // DW3000 stört.
  pinMode(LoRa_chipselect, OUTPUT);
  digitalWrite(LoRa_chipselect, HIGH);
  pinMode(SPI_chipselect, OUTPUT);
  SPI.begin();

#if defined(DEBUG) || defined(BENCH_ON) || defined(COORDINATOR)
  Serial.begin(9600);
  while (!Serial) {};
#endif

  m_id = build_id();
  m_dw_interface = DW3000_Interface(m_id, dw_receive_interrupt_handler);
  m_report = TRIA_MeasureReport(m_id, TRIA_ID(coordinator));

  VERIFY(m_rf95.init());
  // muss NACH m_rf95.init() gemacht werden, weil dort
  // die Default config gesetzt wird
  m_rf95.setModemConfig(RH_RF95::Bw500Cr45Sf128);
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

void ComponentBridge::process_new_lora_messages() {
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
      m_report.reset();
      TRIA_RangeRequest repeated = TRIA_RangeRequest(m_id, TRIA_ID(tracker));
      m_dw_interface.send_range_request(repeated);
    }

    if (received->is_type(measure_report) && m_id.is_coordinator()) {
      m_usb_interface.send_measurement(*static_cast<TRIA_MeasureReport *>(received));
    }
  }
}

uint64_t overflow_safe_timediff(uint64_t future, uint64_t past) {
  return future >= past ? future - past : 0xffffffffff - past + future;
}

void ComponentBridge::process_new_dw_messages() {
  while (m_dw_interface.unprocessed_messages_pending()) {
    BinaryMessage message = m_dw_interface.get_first_unprocessed_message();

    TRIA_GenericPacket *received = nullptr;
    if (!deserialise_packet(message.data, message.data_length, m_id, &received)) {
      continue;
    }
    VERIFY(received != nullptr);

#ifdef DEBUG
    // FIXME: aus irgendeinem Grund kommt mit -D DEBUG die Range Response des Coordinator nicht beim
    // Trackee an. pls investigate
    Serial.print("Paket empfangen (DW): ");
    received->print();
    Serial.print("\n");
#endif

    if (received->is_type(range_request)) {
      auto response =
          TRIA_RangeResponse(m_id, received->received_from(), TRIA_Stamp(message.receive_time));
      m_dw_interface.send_range_response(response);
    } else if (received->is_type(range_response)) {
      TRIA_Stamp measured_rx = static_cast<TRIA_RangeResponse *>(received)->get_rx_stamp();
      TRIA_Stamp measured_tx = static_cast<TRIA_RangeResponse *>(received)->get_tx_stamp();

      // äquivalent zum addieren von 34cm zur resultierenden Distanz
      // berechnet durch 0.34m * (1000000000000 / 15.65ps) / c_air
      static const uint8_t tof_pad = 73;
      uint64_t tof =
          overflow_safe_timediff(message.receive_time, m_dw_interface.get_tx_stamp().value()) +
          tof_pad;
      uint64_t compute_time = overflow_safe_timediff(measured_tx.value(), measured_rx.value());
      uint64_t adjusted_tof = tof >= compute_time ? (tof - compute_time) / 2 : 0;

      auto measure = TRIA_Measure(received->received_from(), TRIA_Stamp(adjusted_tof));
      m_report.add_measurement(measure);

      if (m_report.finished()) {
        send_packet_over_lora(m_report);
        m_report.reset();
      }
    }
  }
}

void ComponentBridge::send_packet_over_lora(TRIA_GenericPacket &packet) {
  packet.pack_into(m_send_buffer);
  VERIFY(m_rf95.send(m_send_buffer, packet.packed_size()));

#ifdef DEBUG
  Serial.print("Paket gesendet (LoRa): ");
  packet.print();
  Serial.print("\n");
#endif
}

void ComponentBridge::receive_dw_message(const dwt_cb_data_t *cb_data) {
  m_dw_interface.store_received_message(cb_data);
  dwt_writefastCMD(CMD_RX);
}

void ComponentBridge::execute() {
  process_new_dw_messages();
  process_new_lora_messages();

  if (m_id.is_coordinator() && m_usb_interface.measurement_requested()) {
    TRIA_RangeRequest request = TRIA_RangeRequest(m_id, TRIA_ID(trackee));
    send_packet_over_lora(request);
  }
}
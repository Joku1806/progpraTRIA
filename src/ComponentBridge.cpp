
#include <TRIA_helper.h>
#include <lib/assertions.h>
#include <packets/TRIA_DataPong.h>
#include <packets/TRIA_MeasureReport.h>
#include <packets/TRIA_Ping.h>
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

#if defined(DEBUG_ON) || defined(BENCH_ON) || defined(COORDINATOR)
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

#ifdef DEBUG_ON
    Serial.print("Paket empfangen (LoRa): ");
    received->print();
    Serial.print("\n");
#endif

    if (received->is_type(ping) && received->received_from().is_coordinator()) {
      if (m_id.is_trackee()) {
        m_report.reset();
      } else if (m_id.is_tracker()) {
        TRIA_Ping start = TRIA_Ping(m_id, TRIA_ID(trackee));
        m_dw_interface.send_ping(start);
      }
    }

    if (received->is_type(measure_report) && m_id.is_coordinator()) {
      m_usb_interface.send_measurement(*static_cast<TRIA_MeasureReport *>(received));
    }
  }
}

void ComponentBridge::process_new_dw_messages() {
  while (m_dw_interface.unprocessed_messages_pending()) {
    BinaryMessage message = m_dw_interface.get_first_unprocessed_message();

    TRIA_GenericPacket *received = nullptr;
    if (!deserialise_packet(message.data, message.data_length, m_id, &received)) {
      continue;
    }
    VERIFY(received != nullptr);

#ifdef DEBUG_ON
    // FIXME: aus irgendeinem Grund kommt mit -D DEBUG_ON die Range Response des Coordinator nicht beim
    // Trackee an. pls investigate
    Serial.print("Paket empfangen (DW): ");
    received->print();
    Serial.print("\n");
#endif

    if (received->is_type(ping)) {
      if (static_cast<TRIA_Ping *>(received)->is_first()) {
        auto response = TRIA_Ping(m_id, received->received_from());
        response.increment_sequence();
        m_dw_interface.send_ping(response);

        TRIA_Stamp timediff_D = m_dw_interface.get_tx_stamp() - message.rx_stamp;
        m_dw_interface.set_timediff_D(timediff_D);
      } else {
        auto response = TRIA_DataPong(m_id, received->received_from());
        m_dw_interface.send_data_pong(response, message.rx_stamp);
      }
    } else if (received->is_type(data_pong)) {
      TRIA_Stamp timediff_R = message.rx_stamp - m_dw_interface.get_tx_stamp();
      m_dw_interface.set_timediff_R(timediff_R);

      uint64_t tracker_timediff_R =
          static_cast<TRIA_DataPong *>(received)->get_timediff_R().value();
      uint64_t tracker_timediff_D =
          static_cast<TRIA_DataPong *>(received)->get_timediff_D().value();
      uint64_t own_timediff_R = m_dw_interface.get_timediff_R().value();
      uint64_t own_timediff_D = m_dw_interface.get_timediff_D().value();

      uint64_t prod_R, prod_D, diff_RD, sum_all, tof;

      // Wenn einer dieser Checks fehlschlägt, haben wir seeeeehr lange Verarbeitungszeiten von
      // Nachrichten. Das sollte niemals vorkommen, deswegen einfach abstürzen.
      if (__builtin_mul_overflow(tracker_timediff_R, own_timediff_R, &prod_R) ||
          __builtin_mul_overflow(tracker_timediff_D, own_timediff_D, &prod_D) ||
          __builtin_add_overflow(tracker_timediff_R, tracker_timediff_D, &sum_all) ||
          __builtin_add_overflow(own_timediff_R, sum_all, &sum_all) ||
          __builtin_add_overflow(own_timediff_D, sum_all, &sum_all)) {
        VERIFY_NOT_REACHED();
      }

      // Falls wir bei kleinen Distanzen durch Messungenauigkeiten im Negativbereich landen, einfach
      // in positiven Bereich spiegeln.
      if (__builtin_sub_overflow(prod_R, prod_D, &diff_RD)) {
        diff_RD = prod_D - prod_R;
      }

      tof = diff_RD / sum_all;

#ifdef DEBUG_ON
      Serial.print("Distanz = ");
      Serial.print(tof * 15.65 / 1000000000000.0 * 299709000.0);
      Serial.print("m\n");
#endif

      auto measure = TRIA_Measure(received->received_from(), TRIA_Stamp(tof));
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
  m_rf95.waitPacketSent();

#ifdef DEBUG_ON
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
  process_new_lora_messages();
  process_new_dw_messages();

  if (m_id.is_coordinator() && m_usb_interface.measurement_requested()) {
    TRIA_Ping relayed = TRIA_Ping(m_id, TRIA_ID(0));
    send_packet_over_lora(relayed);

    if (m_id.is_tracker()) {
      TRIA_Ping start = TRIA_Ping(m_id, TRIA_ID(trackee));
      m_dw_interface.send_ping(start);
    }
  }
}
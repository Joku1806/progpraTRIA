#include <stddef.h>
#include <stdint.h>
#include <lib/TRIA_Field.h>
#include <lib/TRIA_Action.h>
#include <lib/TRIA_ID.h>
#include <lib/TRIA_Stamp.h>

class TRIA_Packet : TRIA_Field {
  public:
    TRIA_Packet() {};
    TRIA_Packet(TRIA_Action a, TRIA_ID sid, TRIA_ID rid, TRIA_Stamp rx)
      : m_action(a), m_sender_id(sid), m_receiver_id(rid), m_rx_stamp(rx) {};
    TRIA_Packet(TRIA_Action a, TRIA_ID sid, TRIA_ID rid, TRIA_Stamp rx, TRIA_Stamp tx)
      : m_action(a), m_sender_id(sid), m_receiver_id(rid), m_rx_stamp(rx), m_tx_stamp(tx) {};
    bool is_addressed_to(TRIA_ID id);
    bool is_type(action a);

    TRIA_ID get_sender_id() { return m_sender_id; }
    TRIA_Stamp get_rx_stamp() { return m_rx_stamp; }
    TRIA_Stamp get_tx_stamp() { return m_tx_stamp; }

    void print();

    uint8_t* packed() override;
    size_t packed_size() override;
    void initialise_from_buffer(uint8_t* buffer) override;

    static const size_t MAX_PACKET_SIZE = 13;
  private:
    TRIA_Action m_action;
    TRIA_ID m_sender_id;
    TRIA_ID m_receiver_id;
    TRIA_Stamp m_rx_stamp;
    TRIA_Stamp m_tx_stamp;
    uint8_t m_packed[MAX_PACKET_SIZE];
};
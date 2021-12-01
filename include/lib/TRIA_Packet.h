#include <stddef.h>
#include <stdint.h>
#include <lib/TRIA_Field.h>
#include <lib/TRIA_Action.h>
#include <lib/TRIA_ID.h>
#include <lib/TRIA_Stamp.h>

class TRIA_Packet : TRIA_Field {
  static const size_t MAX_PACKET_SIZE = 13;

  public:
    TRIA_Packet(TRIA_Action action, TRIA_ID receiver_id, TRIA_ID sender_id, TRIA_Stamp rx_stamp, TRIA_Stamp tx_stamp)
      : m_action(action), m_receiver_id(receiver_id), m_sender_id(sender_id),
        m_rx_stamp(rx_stamp), m_tx_stamp(tx_stamp) {};
    bool is_addressed_to(TRIA_ID id);
    bool is_range_request();
    
    uint8_t* packed() override;
    size_t packed_size() override;
  private:
    TRIA_Action m_action;
    TRIA_ID m_receiver_id;
    TRIA_ID m_sender_id;
    TRIA_Stamp m_rx_stamp;
    TRIA_Stamp m_tx_stamp;
    uint8_t m_packed[MAX_PACKET_SIZE];
};
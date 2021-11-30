#include <stdint.h>
#include <lib/TRIA_Protocol.h>

class TRIA_Packet {
  public:
    TRIA_Packet();
    bool is_addressed_to(TRIA_id id);
    void receive_from(TRIA_id id);
    void send_to(TRIA_id id);

  private:
    TRIA_action action;
    TRIA_id sender_id;
    TRIA_id receiver_id;
    uint64_t rx_stamp;
    uint64_t tx_stamp;
};
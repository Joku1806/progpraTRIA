#include <lib/TRIA_Field.h>

enum TRIA_dev_type {
  tracker = 1 << 7,
  coordinator = 1 << 6,
  trackee = 1 << 5,
};

class TRIA_ID : TRIA_Field {
  public:
    TRIA_ID() : m_id(0) {};
    TRIA_ID(TRIA_dev_type type, uint8_t id) : m_id((type << 5) | (id & 0x1f)) {};
    TRIA_dev_type type();
    uint8_t id();
    bool matches_mask(TRIA_ID mask);

    uint8_t* packed() override;
    size_t packed_size() override;
    void initialise_from_buffer(uint8_t* buffer) override;
  private:
    uint8_t m_id;
};
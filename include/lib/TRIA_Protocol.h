#include <stdint.h>

enum TRIA_action {
  range_request = 1,
  range_response = 2,
  range_report = 3,
};

enum TRIA_dev_type {
  tracker = 1 << 7,
  coordinator = 1 << 6,
  trackee = 1 << 5,
};

class TRIA_id {
  public:
    TRIA_id(TRIA_dev_type type, uint8_t id) : m_type(type), m_id(id) {};
    bool matches_mask(TRIA_id mask);
  private:
    TRIA_dev_type m_type;
    uint8_t m_id;
};
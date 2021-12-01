#include <lib/TRIA_Field.h>

enum action {
  range_request = 1,
  range_response = 2,
  range_report = 3,
};

class TRIA_Action : TRIA_Field {
  public:
    TRIA_Action(action a) : m_action(a) {};
    action value();

    uint8_t* packed() override;
    size_t packed_size() override;
  private:
    action m_action;
};
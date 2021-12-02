#include <lib/TRIA_Field.h>

enum action {
  range_request = 0,
  range_response = 1,
  range_report = 2,
};

class TRIA_Action : TRIA_Field {
  public:
    TRIA_Action() : m_action(range_request) {};
    TRIA_Action(action a) : m_action(a) {};
    action value();

    uint8_t* packed() override;
    size_t packed_size() override;
    void initialise_from_buffer(uint8_t* buffer) override;
  private:
    action m_action;
};
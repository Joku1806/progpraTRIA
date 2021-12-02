#include <cstdint>
#include <cstddef>

class TRIA_Field {
  public:
    TRIA_Field() {};
    virtual uint8_t* packed() = 0;
    virtual size_t packed_size() = 0;
    virtual void initialise_from_buffer(uint8_t *buffer) = 0;
};
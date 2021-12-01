#include <cstdint>
#include <cstddef>

class TRIA_Field {
  public:
    TRIA_Field() {};
    TRIA_Field(uint8_t* bytes, size_t length) {};
    virtual uint8_t* packed() = 0;
    virtual size_t packed_size() = 0;
};
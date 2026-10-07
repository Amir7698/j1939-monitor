// j1939_id.hpp - declaration of the J1939Id class (WHAT it does).
//
// TODO (you):
//   1. Include guard: #ifndef J1939MON_J1939_J1939_ID_HPP / #define ... / #endif
//   2. #include <cstdint>
//   3. namespace j1939mon { ... }
//   4. Inside it, the class J1939Id with only the DECLARATIONS:
//        constructor, priority(), pduFormat(), sourceAddress(), rawValue()
//        and the private member uint32_t raw_;
//      (no function bodies here; they go in src/j1939/j1939_id.cpp)

#ifndef J1939MON_J1939_J1939_ID_HPP
#define J1939MON_J1939_J1939_ID_HPP
#include <cstdint>

class J1939Id
{

public:
    J1939Id(uint32_t value);

    uint32_t priority() const;
    uint32_t pduFormat() const;
    uint32_t sourceAddress() const;
    uint32_t pgn() const;
    uint32_t destinationAddress() const;
    bool isPdu1() const;

    uint32_t rawValue() const;

private:
    uint32_t raw_;
};

#endif
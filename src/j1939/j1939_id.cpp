// j1939_id.cpp - implementation of J1939Id (HOW it works).
//
// TODO (you):
//   1. #include "j1939mon/j1939/j1939_id.hpp"
//   2. namespace j1939mon { ... }
//   3. Inside it, the function BODIES, e.g.
//        uint32_t J1939Id::priority() const { ... }


#include "j1939mon/j1939/j1939_id.hpp"

J1939Id::J1939Id(uint32_t value) {
    raw_ = value;
}

uint32_t  J1939Id::priority() const {
return (raw_ >> 26) & 0x7;
}

uint32_t J1939Id::pduFormat() const {
return (raw_ >> 16) & 0xFF;
}

uint32_t J1939Id::sourceAddress() const {
return raw_  & 0xFF;
}
uint32_t J1939Id::rawValue () const {
return raw_;
}
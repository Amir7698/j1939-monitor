// spn.hpp - SPN decoding: physical values from J1939 frames.
#ifndef J1939MON_J1939_SPN_HPP
#define J1939MON_J1939_SPN_HPP

#include "j1939mon/can/can_frame.hpp"

// The result of decoding one SPN.
struct SpnValue
{
    bool available; // false if the ECU sent "not available" (all ones)
    double value;   // physical value, valid only if available is true
};

SpnValue decodeEngineSpeed(CanFrame frame);

#endif
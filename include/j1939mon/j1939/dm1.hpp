// dm1.hpp - DM1 (active diagnostic trouble codes), single-frame version.
#ifndef J1939MON_J1939_DM1_HPP
#define J1939MON_J1939_DM1_HPP

#include "j1939mon/can/can_frame.hpp"

struct Dtc
{
    uint32_t spn;
    uint8_t fmi;
    uint8_t oc;
};

struct Dm1
{
    bool malfunctionLamp;
    bool redStopLamp;
    bool amberWarningLamp;
    bool hasDtc;
    Dtc dtc;
};

Dm1 decodeDm1(CanFrame frame);

#endif
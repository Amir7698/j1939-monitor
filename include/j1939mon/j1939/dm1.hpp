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

// Decodes one DTC from 4 bytes.
Dtc decodeDtc(const uint8_t *bytes);

// Decodes lamps + first DTC from a DM1 payload (single frame or BAM).
Dm1 decodeDm1(const uint8_t *payload);

// Single-frame shortcut: decodes frame.data.
Dm1 decodeDm1(CanFrame frame);

#endif
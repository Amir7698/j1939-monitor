#include "j1939mon/j1939/dm1.hpp"

static bool lampOn(uint8_t lamps, uint8_t shift)
{
    return ((lamps >> shift) & 0x3) == 1;
}

Dtc decodeDtc(const uint8_t *bytes)
{
    Dtc dtc;
    dtc.spn = static_cast<uint32_t>(bytes[0]) | (static_cast<uint32_t>(bytes[1]) << 8) | (static_cast<uint32_t>(bytes[2] & 0xE0) << 11);
    dtc.fmi = bytes[2] & 0x1F;
    dtc.oc = bytes[3] & 0x7F;
    return dtc;
}

Dm1 decodeDm1(const uint8_t *payload)
{
    Dm1 result;
    uint8_t lamps = payload[0];
    result.malfunctionLamp = lampOn(lamps, 6);
    result.redStopLamp = lampOn(lamps, 4);
    result.amberWarningLamp = lampOn(lamps, 2);
    result.dtc = decodeDtc(payload + 2);
    result.hasDtc = !(result.dtc.spn == 0 && result.dtc.fmi == 0);
    return result;
}

Dm1 decodeDm1(CanFrame frame)
{
    return decodeDm1(frame.data);
}

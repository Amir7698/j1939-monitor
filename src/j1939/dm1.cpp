#include "j1939mon/j1939/dm1.hpp"

static bool lampOn(uint8_t lamps, uint8_t shift)
{
    return ((lamps >> shift) & 0x3) == 1;
}

Dm1 decodeDm1(CanFrame frame)
{
    Dm1 result;
    uint8_t lamps = frame.data[0];
    result.malfunctionLamp = lampOn(lamps, 6);
    result.redStopLamp = lampOn(lamps, 4);
    result.amberWarningLamp = lampOn(lamps, 2);
    result.dtc.spn = frame.data[2];
    result.dtc.spn |= (frame.data[3] << 8);
    result.dtc.spn |= (((frame.data[4] >> 5) & 0x7) << 16);
    result.dtc.fmi = (frame.data[4] & 0x1F);
    result.dtc.oc = frame.data[5] & 0x7F;
    result.hasDtc = !(result.dtc.spn == 0 && result.dtc.fmi == 0);
    return result;
}
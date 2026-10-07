#include "j1939mon/j1939/spn.hpp"

SpnValue decodeEngineSpeed(CanFrame frame)
{

    SpnValue result;
    uint32_t raw = frame.data[3] | (frame.data[4] << 8); // little-endian: low byte first
    if (raw >= 0xFF00)
    {
        result.available = false; // all ones: not available
        result.value = 0.0;
        return result;
    }
    result.available = true;
    result.value = raw * 0.125; // 0.125 rpm per bit
    return result;
}

SpnValue decodeCoolantTemp(CanFrame frame)
{

    SpnValue result;
    uint32_t raw = frame.data[0];
    if (raw >= 0xFF)
    {
        result.available = false;
        result.value = 0;
        return result;
    }
    result.available = true;
    result.value = raw - 40.0;
    return result;
}
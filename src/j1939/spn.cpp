#include "j1939mon/j1939/spn.hpp"

SpnValue decodeEngineSpeed(CanFrame frame)
{

    SpnValue result;
    uint32_t raw = frame.data[3] | (frame.data[4] << 8); // little-endian: low byte first
    if (raw == 0xFFFF)
    {
        result.available = false; // all ones: not available
        result.value = 0.0;
        return result;
    }
    result.available = true;
    result.value = raw * 0.125; // 0.125 rpm per bit
    return result;
}

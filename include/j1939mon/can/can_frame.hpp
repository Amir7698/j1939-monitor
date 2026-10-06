#ifndef J1939MON_CAN_CAN_FRAME_HPP
#define J1939MON_CAN_CAN_FRAME_HPP

#include <cstdint>

struct CanFrame
{
    uint32_t id;
    uint8_t len;
    uint8_t data[8];
};

#endif
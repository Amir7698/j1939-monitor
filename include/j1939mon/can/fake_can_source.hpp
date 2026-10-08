#ifndef J1939MON_CAN_FAKE_CAN_SOURCE_HPP
#define J1939MON_CAN_FAKE_CAN_SOURCE_HPP

#include "j1939mon/can/i_can_source.hpp"

class FakeCanSource : public ICanSource
{

public:
    bool read(CanFrame &frame) override
    {
        CanFrame fake = {0x0CF00400, 8, {0xFF, 0xFF, 0xFF, 0x40, 0x1F, 0xFF, 0xFF, 0xFF}};
        frame = fake;
        return true;
    }
};

#endif
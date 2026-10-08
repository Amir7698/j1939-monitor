#ifndef J1939MON_CAN_I_CAN_SOURCE_HPP
#define J1939MON_CAN_I_CAN_SOURCE_HPP

#include "j1939mon/can/can_frame.hpp"

class ICanSource
{
public:
    // virtual CanFrame read() = 0;
    virtual ~ICanSource() {}
    virtual bool read(CanFrame &frame) = 0;
};

#endif
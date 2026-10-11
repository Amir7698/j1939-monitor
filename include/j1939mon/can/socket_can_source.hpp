#ifndef J1939MON_CAN_SOCKET_CAN_SOURCE_HPP
#define J1939MON_CAN_SOCKET_CAN_SOURCE_HPP

#include "j1939mon/can/i_can_source.hpp"

// Reads real CAN frames from a Linux SocketCAN interface (for example vcan0).
class SocketCanSource : public ICanSource
{
public:
    SocketCanSource(const char *interfaceName);
    ~SocketCanSource();

    bool read(CanFrame &frame) override;

private:
    int socket_;
    int epoll_; // watches socket_ so read() can give up after a timeout
};

#endif

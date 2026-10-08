#include "j1939mon/can/socket_can_source.hpp"
#include <iostream>
#include <sys/socket.h>
#include <linux/can.h>
#include <net/if.h>
#include <unistd.h>

// TODO: includes (the same ones you used in kata/socket_read2.cpp)

SocketCanSource::SocketCanSource(const char *interfaceName)
    : socket_(-1)
{
    // TODO 1: get a hose (socket) and keep its number in socket_

    socket_ = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    // std::cout << "Socket = " << s << std::endl;

    // TODO 2: fill the label (sockaddr_can): type CAN, number of interfaceName

    sockaddr_can addr = {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = if_nametoindex(interfaceName);
    // std::cout << "vcan0 index = " << addr.can_ifindex << std::endl;

    // TODO 3: plug the hose into the hole (bind)
    bind(socket_, (sockaddr *)&addr, sizeof(addr));
}

SocketCanSource::~SocketCanSource()
{
    // TODO 5 (later): give the hose back to Linux
}

CanFrame SocketCanSource::read()
{
    CanFrame frame = {};

    // TODO 4 (later): read one frame from the bus and copy it into frame

    return frame;
}

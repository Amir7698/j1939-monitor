#include "j1939mon/can/socket_can_source.hpp"
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <linux/can.h>
#include <net/if.h>
#include <unistd.h>

SocketCanSource::SocketCanSource(const char *interfaceName)
    : socket_(-1)
{

    sockaddr_can addr = {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = if_nametoindex(interfaceName);
    if (addr.can_ifindex == 0)
    {
        throw std::runtime_error("CAN interface not found");
    }

    socket_ = socket(AF_CAN, SOCK_RAW, CAN_RAW);

    bind(socket_, (sockaddr *)&addr, sizeof(addr));
}

SocketCanSource::~SocketCanSource()
{
    if (socket_ >= 0)
    {
        close(socket_);
    }
}

CanFrame SocketCanSource::read()
{
    CanFrame frame = {};
    can_frame raw = {};

    ::read(socket_, &raw, sizeof(raw));
    frame.id = raw.can_id;
    frame.len = raw.can_dlc;
    for (uint8_t i = 0; i < 8; i++)
    {
        frame.data[i] = raw.data[i];
    }

    return frame;
}

#include "j1939mon/can/socket_can_source.hpp"
#include <stdexcept>
#include <iostream>
#include <sys/socket.h>
#include <linux/can.h>
#include <net/if.h>
#include <unistd.h>
#include <sys/epoll.h>

SocketCanSource::SocketCanSource(const char *interfaceName)
    : socket_(-1), epoll_(-1)
{
    sockaddr_can addr = {};
    addr.can_family = AF_CAN;
    addr.can_ifindex = if_nametoindex(interfaceName);
    if (addr.can_ifindex == 0)
    {
        throw std::runtime_error("CAN interface not found");
    }

    socket_ = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (socket_ < 0)
    {
        throw std::runtime_error("cannot open CAN socket");
    }

    int r = bind(socket_, (sockaddr *)&addr, sizeof(addr));
    if (r < 0)
    {
        close(socket_);
        throw std::runtime_error("cannot bind CAN socket");
    }

    // epoll: tell the kernel to watch our socket for incoming frames.
    // A throwing constructor never runs the destructor, so close what we opened.
    epoll_ = epoll_create1(0);
    if (epoll_ < 0)
    {
        close(socket_);
        throw std::runtime_error("cannot create epoll");
    }

    epoll_event ev = {};
    ev.events = EPOLLIN; // wake up when there is something to read
    ev.data.fd = socket_;
    if (epoll_ctl(epoll_, EPOLL_CTL_ADD, socket_, &ev) < 0)
    {
        close(epoll_);
        close(socket_);
        throw std::runtime_error("cannot watch CAN socket");
    }
}

SocketCanSource::~SocketCanSource()
{
    if (epoll_ >= 0)
    {
        close(epoll_);
    }
    if (socket_ >= 0)
    {
        close(socket_);
    }
}

bool SocketCanSource::read(CanFrame &frame)
{
    can_frame raw = {};

    // Wait at most 100 ms for a frame, so the caller can do other work.
    epoll_event ev;
    int ready = epoll_wait(epoll_, &ev, 1, 100);
    if (ready <= 0)
    {
        return false; // 0 = nothing came (timeout), -1 = interrupted (Ctrl+C)
    }

    ssize_t n = ::read(socket_, &raw, sizeof(raw));
    if (n != sizeof(raw))
    {
        return false;
    }

    frame.id = raw.can_id & CAN_EFF_MASK;
    frame.len = raw.can_dlc;
    for (uint8_t i = 0; i < 8; i++)
    {
        frame.data[i] = raw.data[i];
    }

    return true;
}

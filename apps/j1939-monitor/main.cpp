#include <iostream>
#include "j1939mon/j1939/j1939_id.hpp"
#include "j1939mon/can/socket_can_source.hpp"

void PrintOneFrame(ICanSource *source)
{
    CanFrame frame = source->read();
    J1939Id id(frame.id);
    std::cout << "Priority: " << id.priority() << std::endl;
    std::cout << "SA:       " << id.sourceAddress() << std::endl;
}

int main()
{

    SocketCanSource bus("vcan0");
    while (true)
    {
        PrintOneFrame(&bus);
    }



    return 0;
}
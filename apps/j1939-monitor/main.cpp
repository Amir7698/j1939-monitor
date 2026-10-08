#include <iostream>
#include "j1939mon/j1939/j1939_id.hpp"
#include "j1939mon/can/socket_can_source.hpp"
#include "j1939mon/j1939/spn.hpp"

void PrintOneFrame(ICanSource *source)
{
    CanFrame frame = source->read();
    J1939Id id(frame.id);
    // std::cout << "Priority: " << id.priority() << std::endl;
    // std::cout << "SA:       " << id.sourceAddress() << std::endl;
    if (id.pgn() == 0xF004)
    {
        SpnValue rpm = decodeEngineSpeed(frame);
        if (rpm.available)
            std::cout << "rpm: " << rpm.value << std::endl;
        else
            std::cout << "rpm: N/A" << std::endl;
    }
    else if (id.pgn() == 0xFEEE)
    {
        SpnValue temp = decodeCoolantTemp(frame);

        if (temp.available)
            std::cout << "coolant: " << temp.value << " °C" << std::endl;
        else
            std::cout << "coolant: N/A" << std::endl;
    }
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
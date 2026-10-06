#include <iostream>
#include "j1939mon/j1939/j1939_id.hpp"
#include "j1939mon/can/fake_can_source.hpp"

void PrintOneFrame(ICanSource *source)
{
    CanFrame frame = source->read();
    J1939Id id(frame.id);
    std::cout << "Priority: " << id.priority() << std::endl;
    std::cout << "SA:       " << id.sourceAddress() << std::endl;
}

int main()
{

    J1939Id id(0x0CF00400);

    std::cout << "0x" << std::hex << id.priority() << std::endl;

    FakeCanSource fake;
    PrintOneFrame(&fake);

    return 0;
}
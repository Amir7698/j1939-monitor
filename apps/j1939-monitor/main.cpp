#include <iostream>
#include "j1939mon/j1939/j1939_id.hpp"
#include "j1939mon/can/socket_can_source.hpp"
#include "j1939mon/j1939/spn.hpp"
#include <csignal>

volatile std::sig_atomic_t g_stop = 0;

void onSignal(int)
{
    g_stop = 1;
}

void PrintOneFrame(ICanSource *source)
{
    CanFrame frame = {};
    if (!source->read(frame))
    {
        return;
    }
    // std::cout << "raw id: 0x" << std::hex << frame.id << std::dec << std::endl;
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
    struct sigaction sa = {};
    sa.sa_handler = onSignal;
    sigaction(SIGINT, &sa, nullptr);
    sigaction(SIGTERM, &sa, nullptr);
    try
    {
        SocketCanSource bus("vcan0");
        while (!g_stop)
        {
            PrintOneFrame(&bus);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

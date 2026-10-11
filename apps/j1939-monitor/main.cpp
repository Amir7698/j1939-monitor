#include <iostream>
#include "j1939mon/j1939/j1939_id.hpp"
#include "j1939mon/can/socket_can_source.hpp"
#include "j1939mon/j1939/spn.hpp"
#include "j1939mon/j1939/dm1.hpp"
#include "j1939mon/j1939/bam.hpp"
#include <chrono>
#include <map>
#include <csignal>

volatile std::sig_atomic_t g_stop = 0;

void onSignal(int)
{
    g_stop = 1;
}

static uint32_t nowMs()
{
    using namespace std::chrono;
    auto now = steady_clock::now();
    return duration_cast<milliseconds>(now.time_since_epoch()).count();
}

// Prints every DTC in a DM1 payload: 2 lamp bytes, then 4 bytes per DTC.
static void printDm1(const uint8_t *payload, size_t size)
{
    if (payload == nullptr || size < 6) // lamps + at least one DTC
        return;
    Dm1 dm1 = decodeDm1(payload);
    if (!dm1.hasDtc)
    {
        std::cout << "DM1: no active faults" << std::endl;
        return;
    }
    for (size_t i = 2; i + 4 <= size; i += 4)
    {
        Dtc dtc = decodeDtc(payload + i);
        std::cout << "DM1: SPN " << dtc.spn
                  << " FMI " << int(dtc.fmi)
                  << " OC " << int(dtc.oc);
        if (dm1.redStopLamp)
            std::cout << " [red]";
        if (dm1.amberWarningLamp)
            std::cout << " [amber]";
        if (dm1.malfunctionLamp)
            std::cout << " [MIL]";
        std::cout << std::endl;
    }
}

void PrintOneFrame(ICanSource *source, std::map<uint8_t, BamReassembler> &bams)
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
    else if (id.pgn() == 0xFECA)
    {
        printDm1(frame.data, 8); // single-frame DM1
    }
    else if (id.pduFormat() == 0xEC || id.pduFormat() == 0xEB)
    {
        // BAM: one reassembler per sender, created the first time we hear it.
        uint8_t sa = id.sourceAddress();
        auto it = bams.find(sa);
        if (it == bams.end())
            it = bams.emplace(sa, BamReassembler(sa)).first;

        BamReassembler &bam = it->second;
        if (bam.onFrame(frame, nowMs()) && bam.pgn() == 0xFECA)
            printDm1(bam.data(), bam.size()); // multi-DTC DM1
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
        std::map<uint8_t, BamReassembler> bams;
        while (!g_stop)
        {
            PrintOneFrame(&bus, bams);
            uint32_t now = nowMs();
            for (auto &entry : bams)
            {
                if (entry.second.checkTimeout(now))
                    std::cout << "BAM timeout from SA " << int(entry.first) << std::endl;
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

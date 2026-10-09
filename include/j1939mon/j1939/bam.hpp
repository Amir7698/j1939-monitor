// bam.hpp - reassembles one J1939 BAM transfer (TP.CM + TP.DT frames).
#ifndef J1939MON_J1939_BAM_HPP
#define J1939MON_J1939_BAM_HPP

#include <cstdint>
#include <vector>
#include "j1939mon/can/can_frame.hpp"

class BamReassembler
{
public:
    // Feed every frame with the current time in ms.
    // Returns true when a complete message is ready in pgn() / data().
    bool onFrame(const CanFrame &frame, uint32_t nowMs);

    uint32_t pgn() const { return pgn_; }
    const std::vector<uint8_t> &data() const { return data_; }

private:
    void reset();

    bool active_ = false;      // a transfer is in progress
    uint32_t pgn_ = 0;         // what is being sent (e.g. 0xFECA = DM1)
    uint16_t totalSize_ = 0;   // bytes announced in TP.CM
    uint8_t totalPackets_ = 0; // boxes announced in TP.CM
    uint8_t nextSeq_ = 1;      // the box number we expect next
    uint32_t lastMs_ = 0;      // when the last frame arrived
    std::vector<uint8_t> data_;
};

#endif

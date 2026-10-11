// bam.hpp - reassembles one J1939 BAM transfer (TP.CM + TP.DT frames).
#ifndef J1939MON_J1939_BAM_HPP
#define J1939MON_J1939_BAM_HPP

#include <cstdint>
#include "j1939mon/can/can_frame.hpp"
constexpr uint16_t kBamMaxSize = 1785;

class BamReassembler
{
public:
    explicit BamReassembler(uint8_t sourceAddress) : sa_(sourceAddress)
    {
    }
    // Feed every frame with the current time in ms.
    // Returns true when a complete message is ready in pgn() / data().
    bool onFrame(const CanFrame &frame, uint32_t nowMs);
    bool checkTimeout(uint32_t nowMs);

    bool inProgress() const { return active_; }

    uint32_t pgn() const { return pgn_; }
    const uint8_t *data() const { return data_; }
    uint16_t size() const { return size_; }

private:
    void reset();

    bool active_ = false;      // a transfer is in progress
    uint32_t pgn_ = 0;         // what is being sent (e.g. 0xFECA = DM1)
    uint16_t totalSize_ = 0;   // bytes announced in TP.CM
    uint8_t totalPackets_ = 0; // boxes announced in TP.CM
    uint16_t size_ = 0;        // bytes collected so far
    uint8_t nextSeq_ = 1;      // the box number we expect next
    uint32_t lastMs_ = 0;      // when the last frame arrived
    uint8_t data_[kBamMaxSize];
    uint8_t sa_; // the only sender we listen to
};

#endif

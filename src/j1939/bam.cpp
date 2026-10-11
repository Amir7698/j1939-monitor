#include "j1939mon/j1939/bam.hpp"
#include "j1939mon/j1939/j1939_id.hpp"

namespace
{
    const uint32_t kTpCm = 0xEC;     // PDU format of TP.CM (the letter)
    const uint32_t kTpDt = 0xEB;     // PDU format of TP.DT (the boxes)
    const uint8_t kBamControl = 32;  // TP.CM byte 0: "this is a BAM"
    const uint32_t kTimeoutMs = 750; // J1939-21 T1: max gap between packets
}

void BamReassembler::reset()
{
    active_ = false;
    size_ = 0;
}

bool BamReassembler::checkTimeout(uint32_t nowMs)
{
    if (active_ && (nowMs - lastMs_) > kTimeoutMs)
    {
        reset();
        return true;
    }
    return false;
}

bool BamReassembler::onFrame(const CanFrame &frame, uint32_t nowMs)
{
    J1939Id id(frame.id);
    if (frame.len != 8 || id.sourceAddress() != sa_)
    {
        return false;
    }

    // 1. Too long since the last box? Throw the half-built message away.
    checkTimeout(nowMs);

    // 2. The letter: start a new transfer (drops any unfinished one).
    if (id.pduFormat() == kTpCm && frame.data[0] == kBamControl)
    {
        reset();
        totalSize_ = frame.data[1] | (frame.data[2] << 8);
        if (totalSize_ < 9 || totalSize_ > kBamMaxSize)
        {
            return false;
        }
        totalPackets_ = frame.data[3];
        pgn_ = frame.data[5] | (frame.data[6] << 8) | (frame.data[7] << 16);
        if (totalPackets_ != (totalSize_ + 6) / 7)
        {
            return false; // the letter does not add up: ignore it
        }
        active_ = true;
        nextSeq_ = 1;
        lastMs_ = nowMs;
        return false;
    }

    // 3. A box: it must be the one we expect, otherwise give up.
    if (id.pduFormat() == kTpDt && active_)
    {
        if (frame.data[0] != nextSeq_)
        {
            reset(); // missing or repeated box
            return false;
        }
        for (int i = 1; i <= 7 && size_ < totalSize_; ++i)
        {
            data_[size_] = frame.data[i]; // skip the 0xFF padding at the end
            size_++;
        }
        lastMs_ = nowMs;

        if (nextSeq_ == totalPackets_)
        {
            if (size_ != totalSize_)
            {
                reset();
                return false;
            }
            active_ = false;
            return true;
        }
        ++nextSeq_;
    }
    return false;
}

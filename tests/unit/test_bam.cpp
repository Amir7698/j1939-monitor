// test_bam.cpp - unit tests for BAM reassembly (GoogleTest).
#include <gtest/gtest.h>
#include "j1939mon/j1939/bam.hpp"

// DM1 with 2 DTCs = 10 bytes: lamps(2) + SPN 110 FMI 0 OC 1 + SPN 100 FMI 1 OC 2
const CanFrame kCm  = {0x1CECFF00, 8, {0x20, 10, 0, 2, 0xFF, 0xCA, 0xFE, 0x00}};
const CanFrame kDt1 = {0x1CEBFF00, 8, {1, 0x04, 0xFF, 0x6E, 0x00, 0x00, 0x01, 0x64}};
const CanFrame kDt2 = {0x1CEBFF00, 8, {2, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF}};

TEST(BamTest, TwoPacketDm1)
{
    BamReassembler bam;

    EXPECT_FALSE(bam.onFrame(kCm, 0));   // the letter
    EXPECT_FALSE(bam.onFrame(kDt1, 50)); // box 1 of 2
    EXPECT_TRUE(bam.onFrame(kDt2, 100)); // box 2 of 2: complete!

    EXPECT_EQ(bam.pgn(), 0xFECAu);
    ASSERT_EQ(bam.data().size(), 10u); // padding 0xFF not included
    EXPECT_EQ(bam.data()[2], 0x6E);    // first DTC: SPN 110
    EXPECT_EQ(bam.data()[9], 0x02);    // second DTC: OC 2
}

TEST(BamTest, MissingPacket)
{
    BamReassembler bam;

    EXPECT_FALSE(bam.onFrame(kCm, 0));
    EXPECT_FALSE(bam.onFrame(kDt2, 50));  // box 1 was lost: transfer dropped
    EXPECT_FALSE(bam.onFrame(kDt1, 100)); // too late, nothing to complete
}

TEST(BamTest, Timeout)
{
    BamReassembler bam;

    EXPECT_FALSE(bam.onFrame(kCm, 0));
    EXPECT_FALSE(bam.onFrame(kDt1, 50));
    EXPECT_FALSE(bam.onFrame(kDt2, 1000)); // 950 ms gap > 750 ms
}

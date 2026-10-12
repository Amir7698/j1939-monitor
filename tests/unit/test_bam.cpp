// test_bam.cpp - unit tests for BAM reassembly (GoogleTest).
#include <gtest/gtest.h>
#include "j1939mon/j1939/bam.hpp"

// DM1 with 2 DTCs = 10 bytes: lamps(2) + SPN 110 FMI 0 OC 1 + SPN 100 FMI 1 OC 2
const CanFrame kCm = {0x1CECFF00, 8, {0x20, 10, 0, 2, 0xFF, 0xCA, 0xFE, 0x00}};
const CanFrame kDt1 = {0x1CEBFF00, 8, {1, 0x04, 0xFF, 0x6E, 0x00, 0x00, 0x01, 0x64}};
const CanFrame kDt2 = {0x1CEBFF00, 8, {2, 0x00, 0x01, 0x02, 0xFF, 0xFF, 0xFF, 0xFF}};

TEST(BamTest, TwoPacketDm1)
{
    BamReassembler bam(0x00);

    EXPECT_FALSE(bam.onFrame(kCm, 0));   // the letter
    EXPECT_FALSE(bam.onFrame(kDt1, 50)); // box 1 of 2
    EXPECT_TRUE(bam.onFrame(kDt2, 100)); // box 2 of 2: complete!

    EXPECT_EQ(bam.pgn(), 0xFECAu);
    ASSERT_EQ(bam.size(), 10u);     // padding 0xFF not included
    EXPECT_EQ(bam.data()[2], 0x6E); // first DTC: SPN 110
    EXPECT_EQ(bam.data()[9], 0x02); // second DTC: OC 2
}

TEST(BamTest, MissingPacket)
{
    BamReassembler bam(0x00);

    EXPECT_FALSE(bam.onFrame(kCm, 0));
    EXPECT_FALSE(bam.onFrame(kDt2, 50));  // box 1 was lost: transfer dropped
    EXPECT_FALSE(bam.onFrame(kDt1, 100)); // too late, nothing to complete
}

TEST(BamTest, Timeout)
{
    BamReassembler bam(0x00);

    EXPECT_FALSE(bam.onFrame(kCm, 0));
    EXPECT_FALSE(bam.onFrame(kDt1, 50));
    EXPECT_FALSE(bam.onFrame(kDt2, 1000)); // 950 ms gap > 750 ms
}

TEST(BamTest, ShortFrameIgnored)
{
    BamReassembler bam(0x00);
    CanFrame shortDt1 = kDt1;
    shortDt1.len = 5;
    EXPECT_FALSE(bam.onFrame(kCm, 0));
    EXPECT_FALSE(bam.onFrame(shortDt1, 50));
    EXPECT_FALSE(bam.onFrame(kDt1, 60));
    EXPECT_TRUE(bam.onFrame(kDt2, 100));
}

TEST(BamTest, SizeTooSmall)
{
    BamReassembler bam(0x00);
    CanFrame smallCm = kCm;
    smallCm.data[1] = 5;
    smallCm.data[2] = 0;
    smallCm.data[3] = 1;
    EXPECT_FALSE(bam.onFrame(smallCm, 0));
    EXPECT_FALSE(bam.onFrame(kDt1, 50));
}
TEST(BamTest, DuplicatePacket)
{
    BamReassembler bam(0x00);

    EXPECT_FALSE(bam.onFrame(kCm, 0));
    EXPECT_FALSE(bam.onFrame(kDt1, 50));  // packet 1
    EXPECT_FALSE(bam.onFrame(kDt1, 60));  // duplicate packet 1
    EXPECT_FALSE(bam.onFrame(kDt2, 100)); // transfer was dropped: nothing to complete
}

TEST(BamTest, OtherSenderIgnored)
{
    BamReassembler bam(0x00);

    CanFrame otherDt2 = kDt2;
    otherDt2.id = 0x1CEBFF01; // different source address: 0x01

    EXPECT_FALSE(bam.onFrame(kCm, 0));
    EXPECT_FALSE(bam.onFrame(kDt1, 50));
    EXPECT_FALSE(bam.onFrame(otherDt2, 75)); // ignored: wrong sender
    EXPECT_TRUE(bam.onFrame(kDt2, 100));     // correct sender completes transfer

    EXPECT_EQ(bam.size(), 10u);
}

TEST(BamTest, CheckTimeoutWithoutFrames)
{
    BamReassembler bam(0x00);

    bam.onFrame(kCm, 0);
    bam.onFrame(kDt1, 50);
    EXPECT_TRUE(bam.inProgress());

    EXPECT_FALSE(bam.checkTimeout(500)); // 450 ms since box 1: still fine
    EXPECT_TRUE(bam.inProgress());

    EXPECT_TRUE(bam.checkTimeout(900)); // 850 ms > 750 ms: dropped
    EXPECT_FALSE(bam.inProgress());
}

TEST(BamTest, InconsistentPacketCount)
{
    BamReassembler bam(0x00);
    CanFrame badCm = kCm;
    badCm.data[3] = 3; // 10 bytes need 2 packets, not 3

    EXPECT_FALSE(bam.onFrame(badCm, 0));
    EXPECT_FALSE(bam.inProgress()); // announcement rejected
    EXPECT_FALSE(bam.onFrame(kDt1, 50));
}

TEST(BamTest, SizeTooBig)
{
    BamReassembler bam(0x00);
    CanFrame bigCm = kCm;
    bigCm.data[1] = 0xFA; // 0x06FA = 1786 bytes, one more than the BAM maximum
    bigCm.data[2] = 0x06;
    bigCm.data[3] = 0xFF;

    EXPECT_FALSE(bam.onFrame(bigCm, 0));
    EXPECT_FALSE(bam.inProgress());
}

TEST(BamTest, NewAnnouncementRestartsTransfer)
{
    BamReassembler bam(0x00);

    EXPECT_FALSE(bam.onFrame(kCm, 0));
    EXPECT_FALSE(bam.onFrame(kDt1, 50));
    EXPECT_FALSE(bam.onFrame(kCm, 100)); // new TP.CM: the old transfer is dropped
    EXPECT_FALSE(bam.onFrame(kDt2, 150)); // box 2 no longer fits: we expect box 1 again

    EXPECT_FALSE(bam.onFrame(kCm, 200)); // a clean transfer completes normally
    EXPECT_FALSE(bam.onFrame(kDt1, 250));
    EXPECT_TRUE(bam.onFrame(kDt2, 300));
    EXPECT_EQ(bam.size(), 10u);
}
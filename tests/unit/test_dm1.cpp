// test_dm1.cpp - unit tests for DM1 decoding (GoogleTest).
#include <gtest/gtest.h>
#include "j1939mon/j1939/dm1.hpp"

TEST(Dm1Test, NoFault)
{
    CanFrame frame = {0x18FECA00, 8, {0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF}};
    Dm1 dm1 = decodeDm1(frame);
    EXPECT_FALSE(dm1.hasDtc);
    EXPECT_FALSE(dm1.malfunctionLamp);
    EXPECT_FALSE(dm1.redStopLamp);
    EXPECT_FALSE(dm1.amberWarningLamp);
}

TEST(Dm1Test, CoolantTooHot)
{
    CanFrame frame = {0x18FECA00, 8, {0x04, 0xFF, 0x6E, 0x00, 0x00, 0x01, 0xFF, 0xFF}};
    Dm1 dm1 = decodeDm1(frame);

    EXPECT_TRUE(dm1.amberWarningLamp);
    EXPECT_FALSE(dm1.redStopLamp);
    EXPECT_FALSE(dm1.malfunctionLamp);
    EXPECT_TRUE(dm1.hasDtc);
    EXPECT_EQ(dm1.dtc.spn, 110u); // 0x6E
    EXPECT_EQ(dm1.dtc.fmi, 0);
    EXPECT_EQ(dm1.dtc.oc, 1);
}

TEST(Dm1Test, SpnHighBits)
{
    // byte 4 = 0xE3 = 111 00011: top 3 bits are SPN bits 16-18, low 5 bits are FMI
    CanFrame frame = {0x18FECA00, 8, {0x10, 0xFF, 0x00, 0x00, 0xE3, 0x05, 0xFF, 0xFF}};
    Dm1 dm1 = decodeDm1(frame);

    EXPECT_TRUE(dm1.redStopLamp);
    EXPECT_FALSE(dm1.amberWarningLamp);
    EXPECT_FALSE(dm1.malfunctionLamp);
    EXPECT_TRUE(dm1.hasDtc);
    EXPECT_EQ(dm1.dtc.spn, 458752u); // 7 << 16
    EXPECT_EQ(dm1.dtc.fmi, 3);
    EXPECT_EQ(dm1.dtc.oc, 5);
}

TEST(Dm1Test, DtcFromBytes)
{
    const uint8_t bytes[4] = {0x64, 0x00, 0x01, 0x02};

    Dtc dtc = decodeDtc(bytes);

    EXPECT_EQ(dtc.spn, 100u);
    EXPECT_EQ(dtc.fmi, 1);
    EXPECT_EQ(dtc.oc, 2);
}

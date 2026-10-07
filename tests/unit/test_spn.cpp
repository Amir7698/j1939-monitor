// test_spn.cpp - unit tests for SPN decoding (GoogleTest).
#include <gtest/gtest.h>
#include "j1939mon/j1939/spn.hpp"

TEST(SpnTest, EngineSpeed_1000rpm)
{
    CanFrame frame = {0x0CF00400, 8, {0xFF, 0xFF, 0xFF, 0x40, 0x1F, 0xFF, 0xFF, 0xFF}};

    SpnValue speed = decodeEngineSpeed(frame);

    EXPECT_TRUE(speed.available);          // the value is present
    EXPECT_DOUBLE_EQ(speed.value, 1000.0); // 0x1F40 * 0.125 = 1000 rpm
}

TEST(SpnTest, EngineSpeed_4000rpm)
{
    CanFrame frame = {0x0CF00400, 8, {0xFF, 0xFF, 0xFF, 0x00, 0x7D, 0xFF, 0xFF, 0xFF}};
    SpnValue speed = decodeEngineSpeed(frame);
    EXPECT_TRUE(speed.available);
    EXPECT_DOUBLE_EQ(speed.value, 4000.0);
}

TEST(SpnTest, EngineSpeed_NotAvailable)
{
    CanFrame frame = {0x0CF00400, 8, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    SpnValue speed = decodeEngineSpeed(frame);
    EXPECT_FALSE(speed.available);
}

TEST(SpnTest, EngineSpeed_NotAvailableRange)
{
    CanFrame frame = {0x0CF00400, 8, {0xFF, 0xFF, 0xFF, 0x40, 0xFF, 0xFF, 0xFF, 0xFF}};
    SpnValue speed = decodeEngineSpeed(frame);
    EXPECT_FALSE(speed.available);
}

TEST(SpnTest, CoolantTemp_90C)
{
    CanFrame frame = {0x18FEEE00, 8, {0X82, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    SpnValue temp = decodeCoolantTemp(frame);
    EXPECT_TRUE(temp.available);
    EXPECT_DOUBLE_EQ(temp.value, 90.0);
}

TEST(SpnTest, CoolantTemp_NotAvailable)
{
    CanFrame frame = {0x18FEEE00, 8, {0XFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};
    SpnValue temp= decodeCoolantTemp(frame);
    EXPECT_FALSE(temp.available);
}
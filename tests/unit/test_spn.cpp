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

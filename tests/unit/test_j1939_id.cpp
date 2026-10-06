// test_j1939_id.cpp - unit tests for J1939Id (GoogleTest).
//
// TODO (you, in phase 2):
//   #include <gtest/gtest.h>
//   #include "j1939mon/j1939/j1939_id.hpp"
//   TEST(J1939IdTest, DecodesEec1) { ... EXPECT_EQ(...); }

#include <gtest/gtest.h>
#include "j1939mon/j1939/j1939_id.hpp"

TEST (J1939IdTest, DecodesPriority) {

    J1939Id id(0x0CF00400);
    EXPECT_EQ(id.priority(), 3u);
}

TEST(J1939IdTest, DecodesPduFormat) {

    J1939Id id(0x0CF00400);
    EXPECT_EQ(id.pduFormat(),0xF0u );
}

TEST(J1939IdTest, DecodesSourceAddress) {
    J1939Id id(0x0CF00400);
    EXPECT_EQ(id.sourceAddress(), 0x00u);   
}

TEST(J1939IdTest, DecodesBrakeMessage) {
    J1939Id id(0x18FEF10B);                   
    EXPECT_EQ(id.sourceAddress(), 0x0Bu);    
}
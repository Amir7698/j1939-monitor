cmake_minimum_required(VERSION 3.16)
project(j1939mon VERSION 0.1.0 LANGUAGES CXX)

# Product code: C++11, as required by the job ads.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(J1939MON_WARNINGS -Wall -Wextra -Wpedantic)

# ---------------------------------------------------------------------------
# Library: all reusable code. Add every new src/*.cpp file here.
# ---------------------------------------------------------------------------
add_library(j1939mon
    src/j1939/j1939_id.cpp
    src/j1939/spn.cpp
    src/can/socket_can_source.cpp
)
target_include_directories(j1939mon PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_options(j1939mon PRIVATE ${J1939MON_WARNINGS})

# ---------------------------------------------------------------------------
# Application
# ---------------------------------------------------------------------------
add_executable(j1939-monitor apps/j1939-monitor/main.cpp)
target_link_libraries(j1939-monitor PRIVATE j1939mon)
target_compile_options(j1939-monitor PRIVATE ${J1939MON_WARNINGS})

# ---------------------------------------------------------------------------
# Unit tests (GoogleTest). GoogleTest >= 1.13 needs C++14, so only the
# test executable uses C++14; the library itself stays C++11.
# ---------------------------------------------------------------------------
include(CTest)
if(BUILD_TESTING)
    find_package(GTest REQUIRED)
    add_executable(unit_tests
        tests/unit/test_j1939_id.cpp
        tests/unit/test_spn.cpp
    )
    set_target_properties(unit_tests PROPERTIES CXX_STANDARD 14)
    target_link_libraries(unit_tests PRIVATE j1939mon GTest::gtest GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(unit_tests)
endif()

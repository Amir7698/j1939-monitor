#include <iostream>

#include "j1939mon/j1939/j1939_id.hpp"

int main () {

    J1939Id id(0x0CF00400);

    std::cout<< "0x" << std::hex << id.priority() << std::endl;

    return 0 ;
}
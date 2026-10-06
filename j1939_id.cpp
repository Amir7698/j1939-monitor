#include <iostream>
#include <memory>


class J1939Id {

    public: 

    J1939Id(unsigned int value) {
        raw = value;
    }
    ~J1939Id() {
    std::cout << "Destroyed: 0x" << std::hex << raw << std::endl;
    }
    unsigned int priority () const { //// const method: it does not modify any member of the object, private or public. It only reads.
       return (raw >> 26) & 0x7;
    }

    unsigned int pduFormat() const {
        return (raw >> 16) & 0xFF;
    }
    
    unsigned int sourceAddress() const {
        return raw  & 0xFF;
    }
    unsigned int rawValue () const {
        return raw;
    }
    private:

    unsigned int raw;
};

int main () {



    // J1939Id id;
    // J1939Id id(0x0CF00400);
    J1939Id id(0x0CF00400);
    // id.raw = 0x0CF00400;

    // std::cout << id.raw ;
    std::cout << "0x" << std::hex << id.rawValue() << std::endl;
    std::cout << "0x" << std::hex << id.priority() << std::endl;
    std::cout << "0x" << std::hex << id.pduFormat() << std::endl;
    std::cout << "0x" << std::hex << id.sourceAddress() << std::endl;

    // J1939Id* p = new J1939Id(0x18FEF10B);

    // delete p;

    std::unique_ptr <J1939Id> p (new J1939Id(0x18FEF10B));
    
    std::cout << "0x" << std::hex << p->sourceAddress() << std::endl;

    return 0;

}
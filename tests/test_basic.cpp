#include "test_utils.h"
#include "tape.h"
#include "file_tape.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>

// #define DISABLED

TEST(basic) {
    const int TAPE_LENGTH = 1000;
    {
        FileTape tape_one = FileTape("tape_1.txt", TAPE_LENGTH);
        assert(tape_one.getSize() == TAPE_LENGTH);
        for(int i = 0; !tape_one.isCurrentPosEnd(); i++, tape_one.nextPos()) {
            tape_one.writeAtCurrentPos(i);
        }
    }
    std::ifstream file("tape_1.txt");
    
    assert(file.good());

    for(int i = 0; i < TAPE_LENGTH; i++) {
        int value;
        file >> value;
        assert(value == i);
        assert(file.peek() == ',');
        file.ignore(1);
    }
    assert(file.peek() == 'X');
}

int main() {
    #ifndef DISABLED
    RUN_TEST(basic);
    #else
    std::cout << "TEST DISABLED" << std::endl;
    #endif
    return 0;
}
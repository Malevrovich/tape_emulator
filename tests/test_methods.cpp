#include "test_utils.h"
#include "tape.h"
#include "file_tape.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <exception>

// #define DISABLED

TEST(read_write) {
    FileTape tape_one("tape_1.txt", 1);

    for(int i = -10; i < 11; i++) {
        tape_one.writeAtCurrentPos(i);
        assert(tape_one.readFromCurrentPos() == i);
    }
}

TEST(next_prev) {
    FileTape tape_one("tape_1.txt", 2);

    tape_one.writeAtCurrentPos(1);
    tape_one.nextPos();
    tape_one.writeAtCurrentPos(2);
    assert(tape_one.readFromCurrentPos() == 2);
    tape_one.prevPos();
    assert(tape_one.readFromCurrentPos() == 1);

    try{
        tape_one.prevPos();
        assert(false);
    } catch(std::out_of_range) {
    }

    FileTape tape_two("tape_2.txt", 2);
    tape_two.nextPos();
    tape_two.nextPos();
    try{
        tape_two.nextPos();
        assert(false);
    } catch(std::out_of_range) {
    }
}

TEST(begin_end) {
    FileTape tape_one("tape_1.txt", 2);

    assert(tape_one.isCurrentPosBegin());
    assert(!tape_one.isCurrentPosEnd());
    tape_one.nextPos();
    assert(!tape_one.isCurrentPosBegin());
    assert(!tape_one.isCurrentPosEnd());
    tape_one.nextPos();
    assert(!tape_one.isCurrentPosBegin());
    assert(tape_one.isCurrentPosEnd());
    tape_one.prevPos();
    assert(!tape_one.isCurrentPosBegin());
    assert(!tape_one.isCurrentPosEnd());
    tape_one.prevPos();
    assert(tape_one.isCurrentPosBegin());
    assert(!tape_one.isCurrentPosEnd());
}

TEST(rewind) {
    FileTape tape_one("tape_1.txt", 1000);

    tape_one.goToStart();
    assert(tape_one.isCurrentPosBegin());

    tape_one.goToEnd();
    assert(tape_one.isCurrentPosEnd());

    tape_one.goToStart();
    assert(tape_one.isCurrentPosBegin());

    tape_one.nextPos();
    tape_one.nextPos();
    tape_one.nextPos();

    tape_one.goToStart();
    assert(tape_one.isCurrentPosBegin());

    tape_one.nextPos();
    tape_one.nextPos();
    tape_one.nextPos();

    tape_one.goToEnd();
    assert(tape_one.isCurrentPosEnd());
}

TEST(move) {
    FileTape tape_one("tape_1.txt", 2);
    tape_one.writeAtCurrentPos(1);
    tape_one.nextPos();
    tape_one.writeAtCurrentPos(2);
    
    FileTape tape_two = std::move(tape_one);
    assert(tape_two.getSize() == 2);
    assert(tape_two.readFromCurrentPos() == 2);
    tape_two.prevPos();
    assert(tape_two.readFromCurrentPos() == 1);
    
    {
        FileTape tape_three(std::move(tape_two));
        assert(tape_three.getSize() == 2); 
        assert(tape_three.readFromCurrentPos() == 1);
        tape_three.nextPos();
        assert(tape_three.readFromCurrentPos() == 2);
        tape_two = std::move(tape_three);
    }

    assert(tape_two.getSize() == 2);
    assert(tape_two.readFromCurrentPos() == 2);
    tape_two.prevPos();
    assert(tape_two.readFromCurrentPos() == 1);
}

TEST(reopen) {
    {
        FileTape tape_one("tape_1.txt", 5);
        for(int i = 0; !tape_one.isCurrentPosEnd(); i++, tape_one.nextPos()) {
            tape_one.writeAtCurrentPos(i);
        }
    }
    {
        FileTape tape_reopened = FileTape::parseFile("tape_1.txt");
        assert(tape_reopened.getSize() == 5);
        for(int i = 0; !tape_reopened.isCurrentPosEnd(); i++, tape_reopened.nextPos()) {
            assert(tape_reopened.readFromCurrentPos() == i);
        }
        tape_reopened.prevPos();
        tape_reopened.writeAtCurrentPos(100000);
    }
    {
        FileTape tape_reopened = FileTape::parseFile("tape_1.txt");
        assert(tape_reopened.getSize() == 5);
        for(int i = 0; i < 4; i++, tape_reopened.nextPos()) {
            assert(tape_reopened.readFromCurrentPos() == i);
        }
        assert(tape_reopened.readFromCurrentPos() == 100000);
    }
}

int main() {
    #ifndef DISABLED
    RUN_TEST(read_write);
    RUN_TEST(next_prev);
    RUN_TEST(begin_end);
    RUN_TEST(rewind);
    RUN_TEST(move);
    RUN_TEST(reopen);
    #else
    std::cout << "TEST DISABLED" << std::endl;
    #endif
    return 0;
}
#include "test_utils.h"
#include "tape.h"
#include "file_tape.h"
#include "tape_utils.h"

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <cstdint>
#include <algorithm>
#include <cstdlib>

// #define DISABLED

TEST(copy) {
    FileTape tape_one("tape_1.txt", 5);
    tape_one.writeAtCurrentPos(1);
    tape_one.nextPos();
    tape_one.nextPos();
    tape_one.writeAtCurrentPos(3);
    {
        FileTape tape_two("tape_2.txt", 5);
        TapeCopier::copy(tape_one, tape_two);
        assert(tape_two.getSize() == 5);
        tape_two.goToStart();
        assert(tape_two.readFromCurrentPos() == 1);
        tape_two.nextPos();
        tape_two.nextPos();
        assert(tape_two.readFromCurrentPos() == 3);
    }
    FileTape tape_two = FileTape::parseFile("tape_2.txt");
    assert(tape_one.getSize() == tape_two.getSize());
    tape_one.goToStart();
    tape_two.goToStart();
    assert(tape_one.readFromCurrentPos() == tape_two.readFromCurrentPos());
    tape_one.nextPos();
    tape_one.nextPos();
    tape_two.nextPos();
    tape_two.nextPos();
    assert(tape_one.readFromCurrentPos() == tape_two.readFromCurrentPos());
}

static void assert_equals(const std::vector<int32_t>& arr, Tape& tape) {
    assert(arr.size() == tape.getSize());
    tape.goToStart();
    for(int i = 0; i < arr.size(); i++, tape.nextPos()) {
        assert(arr[i] == tape.readFromCurrentPos());
    }
}

TEST(simple_sort) {
    FileTape tape_one("tape_1.txt", 100);
    for(int i = 100; !tape_one.isCurrentPosEnd(); tape_one.nextPos(), i--) {
        tape_one.writeAtCurrentPos(i);
    }

    TapeSorter sorter{};
    FileTape tape_two("tape_2.txt", 100);
    sorter.sort(tape_one, tape_two);
    tape_two.goToStart();
    for(int i = 1; !tape_two.isCurrentPosEnd(); tape_two.nextPos(), i++) {
        assert(tape_two.readFromCurrentPos() == i);
    }
}

TEST(different_sorts) {
    const int SIZE = 1000;
    srand(100);
    std::vector<int32_t> arr(SIZE);
    
    for(auto &i: arr) i = rand();

    FileTape tape_one("tape_1.txt", SIZE);
    for(int i = 0; i < SIZE; i++, tape_one.nextPos()) {
        tape_one.writeAtCurrentPos(arr[i]);
    }

    sort(arr.begin(), arr.end());
    FileTape tape_two("tape_2.txt", SIZE);

    {
        TapeSorter sorter(SIZE, 2);
        sorter.sort(tape_one, tape_two);
        assert_equals(arr, tape_two);
    }
    {
        TapeSorter sorter(SIZE + 10, 2);
        sorter.sort(tape_one, tape_two);
        assert_equals(arr, tape_two);
    }
    {
        TapeSorter sorter(14, 4);
        sorter.sort(tape_one, tape_two);
        assert_equals(arr, tape_two);
    }
    {
        TapeSorter sorter(10, 3);
        sorter.sort(tape_one, tape_two);
        assert_equals(arr, tape_two);
    }
}

int main() {
    #ifndef DISABLED
    RUN_TEST(copy);
    RUN_TEST(simple_sort);
    RUN_TEST(different_sorts)
    #else
    std::cout << "TEST DISABLED" << std::endl;
    #endif
    return 0;
}
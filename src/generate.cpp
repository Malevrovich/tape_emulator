#include "file_tape.h"
#include "tape_utils.h"

#include <iostream>
#include <string.h>

int main() {
    std::cout << "Name of file: ";
    std::cout.flush();

    std::string filename;
    std::cin >> filename;

    std::cout << "Size: ";
    std::cout.flush();

    size_t size;
    std::cin >> size;

    FileTape tape(filename, size);

    std::cout << "Data: ";
    std::cout.flush();

    for(size_t i = 0; i < size; i++) {
        int value;
        std::cin >> value;
        tape.writeAtCurrentPos(value);
        tape.nextPos();
    }
}
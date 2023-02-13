#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <climits>

#include "tape.h"
#include "file_tape.h"
#include "tape_utils.h"

int main() {   
    std::ifstream config_file("delay.conf");
    FileTapeCongfiguration config;
    if(config_file.good()) {
        try{
            config = FileTapeCongfiguration::parseFile(config_file);
        } catch(std::exception& exception) {
            std::cerr << "Wrong configuration file. Default configuration used" << std::endl;
            std::cerr << exception.what() << std::endl;
            config = FILE_TAPE_DEFAULT_CONFIG;
        }
    } else {
        std::cerr << "Configuration file not found. Default configuration used" << std::endl;
        config = FILE_TAPE_DEFAULT_CONFIG;
    }

    std::string input_filename;
    std::cout << "Input file name: ";
    std::cin >> input_filename;
    
    FileTape in = FileTape::parseFile(input_filename, config);

    std::string output_filename;
    std::cout << "Output file name: ";
    std::cin >> output_filename;

    FileTape out(output_filename, in.getSize(), config);

    TapeSorter sorter{};
    sorter.sort(in, out);

    out.goToStart();
    std::cout << "Sorted: ";
    while(!out.isCurrentPosEnd()) {
        std::cout << out.readFromCurrentPos() << ' ';
        out.nextPos();
    }
    std::cout << std::endl;

    return 0;
}

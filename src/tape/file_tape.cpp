#include "tape.h"
#include "file_tape.h"

#include <algorithm>
#include <thread>
#include <chrono>
#include <fstream>
#include <string>
#include <exception>
#include <iomanip>

#include <iostream>

static const int FILE_TAPE_LENGTH_OF_CELL = 11; // Length of string that contains min/max int32_t number with sign;
static const int FILE_TAPE_LENGTH_OF_SEP = 1;
static const char FILE_TAPE_END = 'X';
static const std::string FILE_TAPE_SEP = ",";

FileTape FileTape::parseFile(const std::string& filename, FileTapeCongfiguration config) {
    std::ifstream file(filename);
    if(!file.good()) {
        throw std::invalid_argument("File " + filename + " not exists");
    }
    int sz = 0;
    while(true) {
        int sep_pos = sz * (FILE_TAPE_LENGTH_OF_CELL + FILE_TAPE_LENGTH_OF_SEP) + FILE_TAPE_LENGTH_OF_CELL;
        file.seekg(sep_pos, std::ios_base::beg);
        for(char c: FILE_TAPE_SEP) {
            if(file.eof() || file.peek() != c) {
                throw std::invalid_argument("Invalid file tape file, expected separator at " + std::to_string(sep_pos));
            }
            file.seekg(1, std::ios_base::cur);
        }
        sz++;
        if(file.peek() == FILE_TAPE_END) break;
    }
    file.close();
    return FileTape(filename, sz, config);
}

FileTape::FileTape(const std::string& filename, size_t size, FileTapeCongfiguration config):
    m_size(size),
    m_config(config),
    m_filename(filename) {
    // We need to create a file and close it if it's not exists
    // if we just fstream(filename) to file that not exists, we won't be able to read from it
    // but we want to read the data that we have written, so we will open and close file to create it
    // and then reopen it with ability to read from it
    std::fstream tmp(filename, std::ios_base::app);
    tmp.close();
    m_file.open(filename, std::ios_base::out | std::ios_base::in);
    
    for(int i = 0; i < m_size; i++) {
        m_file.seekg(i * (FILE_TAPE_LENGTH_OF_CELL + FILE_TAPE_LENGTH_OF_SEP) + FILE_TAPE_LENGTH_OF_CELL, std::ios_base::beg);
        m_file << FILE_TAPE_SEP;
    }
    m_file << FILE_TAPE_END;
    m_file.seekg(0, std::ios_base::beg);
}

FileTape::FileTape(const std::string& filename, size_t size): FileTape(filename, size, FILE_TAPE_DEFAULT_CONFIG) {
}

FileTape::FileTape(FileTape &&other) noexcept: 
    m_size(other.m_size),
    m_config(other.m_config),
    m_filename(other.m_filename),
    m_file(std::move(other.m_file)){
}

FileTape& FileTape::operator=(FileTape &&other) noexcept{
    std::swap(this->m_size, other.m_size);
    std::swap(this->m_config, other.m_config);
    m_file.swap(other.m_file);
    return *this;
}

FileTape::~FileTape() {
    m_file.close();
}

void FileTape::goToStart() {
    std::this_thread::sleep_for(std::chrono::milliseconds(m_config.rewind_delay));
    m_file.seekg(0, std::ios_base::beg);
}

void FileTape::goToEnd() {
    std::this_thread::sleep_for(std::chrono::milliseconds(m_config.rewind_delay));
    m_file.seekg(m_size * (FILE_TAPE_LENGTH_OF_CELL + FILE_TAPE_LENGTH_OF_SEP), std::ios_base::beg);
}

bool FileTape::isCurrentPosEnd() {
    if(m_size == 0) return true;
    return m_file.tellg() == (m_size * (FILE_TAPE_LENGTH_OF_CELL + FILE_TAPE_LENGTH_OF_SEP)); 
}

bool FileTape::isCurrentPosBegin() {
    return m_file.tellg() == 0;
}


void FileTape::nextPos() {
    std::this_thread::sleep_for(std::chrono::milliseconds(m_config.move_delay));
    if(this->isCurrentPosEnd()) {
        throw std::out_of_range("End of the tape " + m_filename + " reached");
    }
    // in std::fstream seekg and seekp are synchronized, so we need to move only one
    m_file.seekg(FILE_TAPE_LENGTH_OF_CELL + FILE_TAPE_LENGTH_OF_SEP, std::ios_base::cur);
}

void FileTape::prevPos() {
    std::this_thread::sleep_for(std::chrono::milliseconds(m_config.move_delay));
    if(this->isCurrentPosBegin()) {
        throw std::out_of_range("Start of the tape reached");
    }
    // in std::fstream seekg and seekp are synchronized, so we need to move only one
    m_file.seekg(-FILE_TAPE_LENGTH_OF_CELL-FILE_TAPE_LENGTH_OF_SEP, std::ios_base::cur);
}

void FileTape::writeAtCurrentPos(int32_t val) {
    std::this_thread::sleep_for(std::chrono::milliseconds(m_config.write_delay));
    m_file << std::showpos << std::internal << std::setw(FILE_TAPE_LENGTH_OF_CELL) << std::setfill('0') << val;
    m_file.flush();
    m_file.seekg(-FILE_TAPE_LENGTH_OF_CELL, std::ios_base::cur);
}

int32_t FileTape::readFromCurrentPos() {
    std::this_thread::sleep_for(std::chrono::milliseconds(m_config.read_delay));
    int32_t res;
    m_file >> res;
    m_file.seekg(-FILE_TAPE_LENGTH_OF_CELL, std::ios_base::cur);
    return res;
}

FileTapeCongfiguration FileTapeCongfiguration::parseFile(std::ifstream& file) {
    FileTapeCongfiguration res = FILE_TAPE_DEFAULT_CONFIG;
    std::string line;

    while(std::getline(file, line)) {
        size_t delim = line.find("=");
        
        std::string key = line.substr(0, delim);
        std::string value_str = line.substr(delim + 1);

        int value = std::max(stoi(value_str), 0);
        if(key == "move_delay") {
            res.move_delay = value;
        } else if(key == "write_delay") {
            res.write_delay = value;
        } else if(key == "read_delay") {
            res.read_delay = value;
        } else if(key == "rewind_delay") {
            res.rewind_delay = value;
        } else {
            throw std::invalid_argument("Invalid configuration field '" + key + "'");
        }
    }

    return res;
}
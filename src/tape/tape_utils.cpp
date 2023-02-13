#include "tape_utils.h"
#include "file_tape.h"

#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <queue>
#include <memory>

#include <iostream>

static const std::string TMP_DIR = "/tmp/";

void TapeCopier::copy(Tape& src, Tape& dst) {
    if(src.getSize() != dst.getSize()) throw std::invalid_argument("Different sizes of tapes, unable to copy");
    for(src.goToStart(), dst.goToStart(); !src.isCurrentPosEnd(); src.nextPos(), dst.nextPos()) {
        dst.writeAtCurrentPos(src.readFromCurrentPos());
    }
}

TapeSorter::TapeSorter(size_t buffer_size, size_t additional_tapes):
    m_additional_tapes(additional_tapes),
    m_buffer_size(buffer_size) {
    m_pool.reserve(additional_tapes - 1);
    for(int i = 0; i < (additional_tapes - 1); i++) {
        auto tape_ptr = std::make_unique<FileTape>(TMP_DIR + "file_tape_" + std::to_string(i), buffer_size);
        tape_ptr->goToEnd();
        m_pool.push_back(std::move(tape_ptr));
    }
}

static void fill_buffer(Tape& tape, std::vector<int32_t>& buffer) {
    int i = 0;
    for(i = 0; i < buffer.size() && !tape.isCurrentPosEnd(); i++, tape.nextPos()) {
        buffer[i] = tape.readFromCurrentPos();
    }
    if(tape.isCurrentPosEnd()) {
        buffer.resize(i);
    }
}

static void fill_tape(std::vector<int32_t>& buffer, Tape& tape) {
    tape.goToStart();
    if(buffer.size() < tape.getSize()) {
        for(int k = 0; k < tape.getSize() - buffer.size(); k++) tape.nextPos();
    }
    int i = 0;
    for(int i = 0; i < buffer.size() && !tape.isCurrentPosEnd(); i++, tape.nextPos()) {
        tape.writeAtCurrentPos(buffer[i]);
    }
    tape.goToStart();
    if(buffer.size() < tape.getSize()) {
        for(int k = 0; k < tape.getSize() - buffer.size(); k++) tape.nextPos();
    }
}

namespace{
    struct node{
        int tape_index;
        int value;
    };

    bool operator<(const node& lhs, const node& rhs) {return lhs.value > rhs.value;}
}
size_t TapeSorter::merge_pool_and_tape(Tape& sorted, size_t written_count, Tape& dst) {
    sorted.goToStart();
    dst.goToStart();

    std::priority_queue<node> nodes;

    for(int i = 0; i < m_pool.size(); i++) {
        if(m_pool[i]->isCurrentPosEnd()) continue;
        nodes.push({i, m_pool[i]->readFromCurrentPos()});
        m_pool[i]->nextPos();
    }
    int sorted_tape_index = 0;
    while(!nodes.empty()) {
        node least = nodes.top();
        nodes.pop();
        if(!m_pool[least.tape_index]->isCurrentPosEnd()) {
            nodes.push({least.tape_index, m_pool[least.tape_index]->readFromCurrentPos()});
            m_pool[least.tape_index]->nextPos();
        }

        if(sorted_tape_index < written_count) {
            int sorted_tape_value = sorted.readFromCurrentPos();
            while(sorted_tape_index < written_count && sorted_tape_value < least.value) {
                dst.writeAtCurrentPos(sorted_tape_value);
                dst.nextPos();

                sorted_tape_index++;
                sorted.nextPos();
                if(sorted_tape_index < written_count) sorted_tape_value = sorted.readFromCurrentPos();
            }
        }

        dst.writeAtCurrentPos(least.value);
        dst.nextPos();
    }

    while(sorted_tape_index < written_count) {
        dst.writeAtCurrentPos(sorted.readFromCurrentPos());
        dst.nextPos();
        sorted_tape_index++;
        if(sorted_tape_index != written_count) sorted.nextPos();
    }
    return written_count + m_pool.size() * m_buffer_size;
} 

void TapeSorter::sort(Tape& src, Tape& dst) {
    if(src.getSize() != dst.getSize()) throw std::invalid_argument("Different size of tapes"); 
    
    src.goToStart();
    dst.goToStart();

    std::vector<int32_t> buffer(m_buffer_size);

    FileTape tmp_tape(TMP_DIR + "file_tape_" + std::to_string(m_additional_tapes - 1), src.getSize());

    bool write_to_tmp = true;
    int current_pool_member = 0;
    size_t written_count = 0;
    while(!src.isCurrentPosEnd()) {
        fill_buffer(src, buffer);
        std::sort(buffer.begin(), buffer.end());
        fill_tape(buffer, *m_pool[current_pool_member]);

        current_pool_member++;
        if(current_pool_member == m_pool.size() || src.isCurrentPosEnd()) {
            write_to_tmp = !write_to_tmp;
            if(write_to_tmp) {
                written_count = merge_pool_and_tape(dst, written_count, tmp_tape);
            } else {
                written_count = merge_pool_and_tape(tmp_tape, written_count, dst);
            }
           current_pool_member = 0;
        }
    }

    if(write_to_tmp) {
        TapeCopier::copy(tmp_tape, dst);
    }
}
#ifndef _TAPE_UTILS_H_
#define _TAPE_UTILS_H_

#include "tape.h"

#include <vector>
#include <memory>

class TapeCopier{
public:
    static void copy(Tape& src, Tape& dst);
};

class TapeSorter{
public:
    TapeSorter(size_t buffer_size=50, size_t additional_tapes=10 /* MIN 2 */ );
    void sort(Tape& src, Tape& dst);
private:
    size_t merge_pool_and_tape(Tape& sorted, size_t written_count, Tape& dst);
    std::vector<std::unique_ptr<Tape>> m_pool;
    size_t m_buffer_size;
    size_t m_additional_tapes;
};

#endif /* _TAPE_UTILS_H_ */

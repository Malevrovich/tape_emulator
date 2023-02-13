#ifndef _TAPE_H_
#define _TAPE_H_

#include <cstdint>

class Tape {
public:
    Tape() = default;

    Tape(const Tape&) = delete;
    Tape(Tape&&) = delete;
    Tape& operator=(const Tape&) = delete;
    Tape& operator=(Tape&&) = delete;

    virtual void nextPos() = 0;
    virtual void prevPos() = 0;
    virtual void goToStart() = 0;
    virtual void goToEnd() = 0;

    virtual bool isCurrentPosBegin() = 0;
    virtual bool isCurrentPosEnd() = 0;

    virtual unsigned int getSize() const = 0;

    virtual void writeAtCurrentPos(int32_t) = 0;
    virtual int readFromCurrentPos() = 0;

    virtual ~Tape() {}
};

#endif /* _TAPE_H_ */

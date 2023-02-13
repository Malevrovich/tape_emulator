#ifndef _FILE_TAPE_H_
#define _FILE_TAPE_H_

#include "tape.h"

#include <cstdint>
#include <fstream>

struct FileTapeCongfiguration{
    unsigned int move_delay; // ms
    unsigned int read_delay; // ms
    unsigned int write_delay; // ms
    unsigned int rewind_delay; // ms

    static FileTapeCongfiguration parseFile(std::ifstream&);
};

static const FileTapeCongfiguration FILE_TAPE_DEFAULT_CONFIG{0U, 0U, 0U};

class FileTape: public Tape {
public:
    FileTape(const std::string& filename, size_t size);
    FileTape(const std::string& filename, size_t size, FileTapeCongfiguration conf);

    static FileTape parseFile(const std::string& filename, FileTapeCongfiguration config=FILE_TAPE_DEFAULT_CONFIG);

    FileTape(const FileTape&) = delete;
    FileTape& operator=(const FileTape&) = delete;

    FileTape(FileTape&&) noexcept;
    FileTape& operator=(FileTape&&) noexcept;

    void nextPos() override;
    void prevPos() override;
    void goToStart() override;
    void goToEnd() override;

    bool isCurrentPosBegin() override;
    bool isCurrentPosEnd() override;

    unsigned int getSize() const override { return m_size; }
    FileTapeCongfiguration getConfig() const { return m_config; }
    
    void writeAtCurrentPos(int32_t) override;
    int readFromCurrentPos() override;

    ~FileTape();

private:
    FileTape();

    FileTapeCongfiguration m_config = FILE_TAPE_DEFAULT_CONFIG;
    std::fstream m_file;
    std::string m_filename;
    size_t m_size;
};

#endif /* _FILE_TAPE_H_ */

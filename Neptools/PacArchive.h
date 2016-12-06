#pragma once

#include <string>
#include <array>
#include <vector>

using namespace std;

namespace neptools {

// structure for headers and index entries documented in ./doc/pac.txt
// header data
struct Header
{
    array<char, 8> id;
    int32_t field_8;
    int32_t entry_count;
    int32_t sequence_number;
};

// entry index data
struct PacIndexEntry
{
    int32_t field_0;
    int32_t file_id;
    array<char, 260> filename;
    int32_t field_10c;
    int32_t stored_size;
    int32_t uncompressed_size;
    int32_t compression_flag;
    int32_t offset;
};

class PacArchive
{
private:
    wstring filename;
    Header header;
    vector<PacIndexEntry> index;
    int data_offset;
public:
    wstring get_filename() const;
    void open(wstring fielpath);
    void reset();

    wstring print_info() const;
    wstring get_header_csv() const;
    wstring get_index_csv() const;

    PacArchive();
    PacArchive(const PacArchive&) = default;
    PacArchive(PacArchive&& other) = default;
    ~PacArchive() = default;
};

}
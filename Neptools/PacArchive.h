#pragma once

#include <array>
#include <vector>
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

using namespace std;

namespace neptools {

    // header data
    struct Header
    {
        array<char, 8> id;
        int32_t field_2; // seems to be always 0
        int32_t entry_count;
        int32_t sequence_number;
    };

    // entry index data
    struct PacEntry
    {
    public:
        int32_t field_0; // magic? maybe always 0
        int32_t file_id;
        array<char,260> filename;
        int32_t field_66; // unknown, magic?
        int32_t compressed_size;
        int32_t uncompressed_size;
        int32_t compression_flag; // seems to be always 1
        int32_t offset; // offset to this entrys fiel data
    };


    class PacArchive
    {
    private:
        fs::path filepath;
        Header header;
        vector<PacEntry> entry_index;
        int32_t data_offset;
    public:
        void open();

        PacArchive() = delete;
        PacArchive(fs::path& pacfile);
        PacArchive(PacArchive&) = delete;
        PacArchive(PacArchive&& other) = default;
        ~PacArchive() = default;
    };

}
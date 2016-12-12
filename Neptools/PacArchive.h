// PacArchive.h
//
// Purpose: Class to open PAC archive files

#pragma once

#include <string>
#include <vector>
#include <boost/filesystem/fstream.hpp>

using namespace std;

namespace neptools {

// Header data
struct header
{
    int entry_count;
    int sequence_number;
};

// Index entry data
struct index_entry
{
    int file_id;
    string filename;
    int stored_size;
    int uncompressed_size;
    bool compression_flag;
    int offset;
};

class pac
{
private:
    string filename;
    header header;
    vector<index_entry> index;
    int data_offset;

public:
    string get_filename() const;
    void open(string fielpath);
    void reset();

    string print_info() const;
    void header_csv(boost::filesystem::ofstream& file) const;
    void index_csv(boost::filesystem::ofstream& file) const;

    pac();
    pac(const pac&) = default;
    pac(pac&& other) = default;
    ~pac() = default;
private:
    bool read_header(boost::filesystem::ifstream& in);
    void read_index(boost::filesystem::ifstream& in);
};

}
// PacArchive.cpp
//
// Copyright 2016 Reto Scheidegger
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stdafx.h"
#include "PacArchive.h"
#include "strtools.hpp"

#include <boost/filesystem.hpp>
#include <sstream>
#include <array>

namespace fs = boost::filesystem;

using namespace std;

namespace neptools {

struct header_raw
{
    array<char, 8> id;
    int32_t field_8;
    int32_t entry_count;
    int32_t sequence_number;
};

struct index_entry_raw
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

pac::pac() :
    filename { "" },
    header { 0, 0 },
    data_offset(0)
{
}

string pac::get_filename() const {
    return filename;
}

void pac::open(string pacfile) {
    this->filename = pacfile;
    fs::path pac_path { pacfile };
    if (pac_path.size() <= 20)
    {
        throw invalid_argument("File " + pac_path.generic_string() + " is not a valid PAC archive. Size too small.\n");
    }
    fs::ifstream ifstream(pac_path, ios_base::binary);
    if (!ifstream.good())
    {
        throw ios_base::failure("Failed to open PAC archive. File: " + pac_path.generic_string() + '\n');
    }

    if (!read_header(ifstream))
    {
        throw invalid_argument("File " + pac_path.generic_string() + " is not a valid PAC archive. ID string does not match.\n");
    }
    index.reserve(header.entry_count);
    read_index(ifstream);
    data_offset = sizeof(header_raw) + header.entry_count * sizeof(index_entry_raw);
}

bool pac::read_header(fs::ifstream& in) {
    header_raw buffer;
    in.read(reinterpret_cast<char*>(&buffer), sizeof(header_raw));
    if (std::string(buffer.id.data()) != "DW_PACK")
    {
        return false;
    }
    header.entry_count = buffer.entry_count;
    header.sequence_number = buffer.sequence_number;
    return true;
}

void pac::read_index(fs::ifstream& in) {
    index_entry_raw buffer;
    index_entry entry;
    for (auto i { 0 }; i < header.entry_count; i++)
    {
        in.read(reinterpret_cast<char*>(&buffer), sizeof(index_entry_raw));
        entry.file_id = buffer.file_id;
        entry.filename = shift_jis2utf8(string(buffer.filename.data()));
        entry.stored_size = buffer.stored_size;
        entry.uncompressed_size = buffer.uncompressed_size;
        entry.compression_flag = buffer.compression_flag;
        entry.offset = buffer.offset;
        index.emplace_back(entry);
    }
}

void pac::reset() {
    filename = "";
    header.entry_count = 0;
    header.sequence_number = 0;
    index = vector<index_entry>();
    data_offset = 0;
}


string pac::print_info() const {
    stringstream info;
    info << "Header data\n";
    info << "PAC Filename : " << filename << '\n';
    info << "Sequenceno.  : " << header.sequence_number << '\n';
    info << "File counts  : " << header.entry_count << '\n';
    info << "\nIndex data\n";
    auto count { 1 };
    for (auto current : index)
    {
        info << "Entry no.   : " << count << '\n';
        info << "ID          : " << current.file_id << '\n';
        info << "Filename    : " << current.filename << '\n';
        info << "Stored size : " << current.stored_size << '\n';
        info << "Uncomp. size: " << current.uncompressed_size << '\n';
        info << "Comp. flag  : " << current.compression_flag << '\n';
        info << "Offset      : " << current.offset << '\n';
        count++;
    }
    return info.str();
}

string pac::header_csv() const {
    stringstream csv;
    csv << '\"' << filename << "\",";
    csv << header.sequence_number << ',';
    csv << header.entry_count << ',';
    return csv.str();
}

string pac::index_csv() const {
    stringstream csv;
    for (auto current : index)
    {
        csv << '\"' << filename << "\",";
        csv << current.file_id << ',';
        csv << '\"' << current.filename << "\",";
        csv << current.stored_size << ',';
        csv << current.uncompressed_size << ',';
        csv << current.compression_flag << ',';
        csv << current.offset << '\n';
    }
    return csv.str();
}
}

// PacArchive.cpp
//
// Project: Neptools
// Author: PixelSupreme
//
// Open and parse header and index data of PAC archive files.

#include "stdafx.h"
#include "PacArchive.h"
#include "strconvert.hpp"

#include <boost/filesystem.hpp>
#include <sstream>
#include <codecvt>

namespace fs = boost::filesystem;

using namespace std;

namespace neptools {


PacArchive::PacArchive() :
    filename{ L"" },
    header{ "", 0, 0, 0 },
    data_offset(0)
{
}

wstring PacArchive::get_filename() const
{
    return filename;
}


void PacArchive::open(wstring pacfile)
{
    this->filename = pacfile;
    fs::path pac_path{ pacfile };
    if (pac_path.size() <= 20)
    {
        throw invalid_argument("Invalid PAC file " + pac_path.generic_string() + ". Size too small.");
    }
    fs::ifstream in(pac_path, std::ios_base::binary);
    if (!in.good())
    {
        throw ios_base::failure("Failed to open PAC archive. File: " + pac_path.generic_string());
    }

    // read archive header
    in.read(reinterpret_cast<char*>(&header), sizeof(Header));
    if (std::string(header.id.data()) != "DW_PACK")
    {
        throw invalid_argument("File " + pac_path.generic_string() + " is not a valid PAC archive. ID string does not match.");
    }

    // read file index entries
    index.reserve(header.entry_count);
    PacIndexEntry entry;
    for (auto i{ 0 }; i < header.entry_count; i++)
    {
        in.read(reinterpret_cast<char*>(&entry), sizeof(PacIndexEntry));
        index.push_back(entry);
    }
    data_offset = sizeof(Header) + header.entry_count * sizeof(PacIndexEntry);
}

void PacArchive::reset()
{
    filename = L"";
    header = Header{ "", 0, 0, 0 };
    index = vector<PacIndexEntry>();
    data_offset = 0;
}


wstring PacArchive::print_info() const
{
    wstringstream info;
    info << L"Header data\n";
    info << L"PAC Filename : " << filename << L'\n';
    info << L"Sequenceno.  : " << header.sequence_number << L'\n';
    info << L"File counts  : " << header.entry_count << L'\n';
    info << L"field_8 value: " << header.field_8 << L'\n';
    info << L"\nIndex data\n";
    auto count{ 1 };
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    for (auto current : index)
    {
        info << L"Entry no.   : " << count << L'\n';
        info << L"ID          : " << current.file_id << L'\n';
        info << L"Filename    : " << converter.from_bytes(current.filename.data()) << L'\n';
        info << L"Stored size : " << current.stored_size << L'\n';
        info << L"Uncomp. size: " << current.uncompressed_size << L'\n';
        info << L"Comp. flag  : " << current.compression_flag << L'\n';
        info << L"Offset      : " << current.offset << L'\n';
        info << L"field_0     : " << current.field_0 << L'\n';
        info << L"Field_10C   : " << current.field_10c << L"\n\n";
        count++;
    }
    return info.str();
}

wstring PacArchive::get_header_csv() const
{
    wstringstream csv;
    csv << L'\"' << filename << L"\",";
    csv << header.sequence_number << L',';
    csv << header.entry_count << L',';
    csv << header.field_8 << L'\n';
    return csv.str();
}

wstring PacArchive::get_index_csv() const
{
    wstringstream csv;
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    
    for (auto current : index)
    {
        csv << L'\"' << filename << L"\",";
        csv << current.field_0 << L',';
        csv << current.file_id << L',';
        csv << L'\"' << shift_jis2utf16(current.filename.data()) << L"\",";
        csv << current.stored_size << L',';
        csv << current.uncompressed_size << L',';
        csv << current.compression_flag << L',';
        csv << current.offset << L',';
        csv << current.field_10c << L'\n';
    }
    return csv.str();
}
}

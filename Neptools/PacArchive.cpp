// PacArchive.cpp
//
// Project: Neptools
// Author: PixelSupreme
//
// Open and parse header and index data of PAC archive files.

#include "stdafx.h"
#include "PacArchive.h"

#include <boost/filesystem.hpp>
#include <sstream>
#include <codecvt>

namespace neptools {

    PacArchive::PacArchive() :
    header({ "", 0, 0, 0 }), data_offset(0)
    {
    }

    void PacArchive::open(const fs::path& pacfile)
    {
        filename = pacfile.filename().wstring();
        if (pacfile.size() <= 20)
        {
            throw std::invalid_argument("Invalid PAC file " + pacfile.generic_string() + ". Size too small.");
        }
        fs::ifstream in(pacfile, std::ios_base::binary);
        if (!in.is_open())
        {
            throw std::ios_base::failure("Failed to open PAC archive. File: " + pacfile.generic_string());
        }
        // read archive header
        in.read(reinterpret_cast<char*>(&header), sizeof(Header));
        if (std::string(header.id.data()) != "DW_PACK")
        {
            throw std::invalid_argument("File " + pacfile.generic_string() + " is not a valid PAC archive. ID string does not match.");
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

    wstring PacArchive::print_info() const
    {
        wstringstream info;
        info << "Header data\n";
        info << "PAC Filename : " << filename << '\n';
        info << "Sequenceno.  : " << header.sequence_number << '\n';
        info << "File counts  : " << header.entry_count << '\n';
        info << "field_8 value: " << header.field_8 << '\n';
        info << "\nIndex data\n";
        auto count{ 1 };
        wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
        for (auto current : index)
        {
            info << "Entry no.   : " << count << '\n';
            info << "ID          : " << current.file_id << '\n';
            info << "Filename    : " << wstring(converter.from_bytes(current.filename.data())) << '\n';
            info << "Stored size : " << current.stored_size << '\n';
            info << "Uncomp. size: " << current.uncompressed_size << '\n';
            info << "Comp. flag  : " << current.compression_flag << '\n';
            info << "Offset      : " << current.offset << '\n';
            info << "field_0     : " << current.field_0 << '\n';
            info << "Field_66    : " << current.field_10c << "\n\n";
            count++;
        }
        return info.str();
    }
}

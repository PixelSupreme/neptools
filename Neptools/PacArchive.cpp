#include "stdafx.h"
#include "PacArchive.h"

namespace neptools {

    PacArchive::PacArchive(fs::path & pacfile) :
    filepath(pacfile), data_offset(0)
    {
    }

    void PacArchive::open()
    {
        if (filepath.size() <= 20)
        {
            throw std::invalid_argument("Invalid PAC file " + filepath.generic_string() + ". Size too small.");
        }
        fs::ifstream in(filepath, std::ios_base::binary);
        if (!in.is_open())
        {
            throw std::ios_base::failure("Failed to open PAC archive. File: " + filepath.generic_string());
        }
        // read archive header
        in.read(reinterpret_cast<char*>(&header), sizeof(Header));
        if (std::string(header.id.data()) != "DW_PACK")
        {
            throw std::invalid_argument("File " + filepath.generic_string() + " is not a valid PAC archive. ID string does not match.");
        }

        // read file index entries
        entry_index.reserve(header.entry_count);
        PacEntry entry;
        for (auto i{ 0 }; i < header.entry_count; i++)
        {
            in.read(reinterpret_cast<char*>(&entry), sizeof(PacEntry));
            entry_index.push_back(entry);
        }
        data_offset = sizeof(Header) + header.entry_count * sizeof(PacEntry);
    }

}
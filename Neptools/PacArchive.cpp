#include "stdafx.h"
#include "PacArchive.h"
#include <sstream>

namespace neptools {

    PacArchive::PacArchive(fs::path pacfile) :
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
        index.reserve(header.entry_count);
        PacIndexEntry entry;
        for (auto i{ 0 }; i < header.entry_count; i++)
        {
            in.read(reinterpret_cast<char*>(&entry), sizeof(PacIndexEntry));
            index.push_back(entry);
        }
        data_offset = sizeof(Header) + header.entry_count * sizeof(PacIndexEntry);
    }

    string PacArchive::print_info() const
    {
        stringstream info;
        info << "Header data\n";
        info << "PAC Filename : " << filepath.filename().string() << '\n';
        info << "Sequenceno.  : " << header.sequence_number << '\n';
        info << "File counts  : " << header.entry_count << '\n';
        info << "field_2 value: " << header.field_2 << '\n';
        info << "\nIndex data\n";
        auto count{ 1 };
        for (auto current : index)
        {
            info << "Entry no.   : " << count << '\n';
            info << "ID          : " << current.file_id << '\n';
            info << "Filename    : " << string(current.filename.data()) << '\n';
            info << "Stored size : " << current.stored_size << '\n';
            info << "Uncomp. size: " << current.uncompressed_size << '\n';
            info << "Comp. flag  : " << current.compression_flag << '\n';
            info << "Offset      : " << current.offset << '\n';
            info << "field_0     : " << current.field_0 << '\n';
            info << "Field_66    : " << current.field_66 << "\n\n";
            count++;
        }
        return info.str();
    }
}

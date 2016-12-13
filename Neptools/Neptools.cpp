// Neptools.cpp : Defines the entry point for the to_console application.
//

#include "stdafx.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "SLog.hpp"
#include "PacArchive.h"
#include <codecvt>
#include "strtools.hpp"

using namespace neptools;
using namespace std;

namespace fs = boost::filesystem;
namespace po = boost::program_options;


struct options
{
    string game_dir;
    string out_dir;
};

log::logger g_log;


void print_help(po::options_description desc) {
    u8print("\n\tUsage: neptools.exe <gamedir> <outdir>\n\n");
    u8print("  <gamedir>\t\tDirectory of the game you want to extract files from.\n");
    u8print("  <outdir>\t\tOutput directory for extracted files.\n\n");
    stringstream buffer;
    buffer << desc;
    u8print(buffer.str());
}

void parse_commandline(int argc, wchar_t** argv, options& options) {
    try
    {
        wstring gamedir;
        wstring outdir;
        po::options_description general("General options");
        general.add_options()
            ("help,h", "Display help message")
            ;
        po::options_description dirs("Input and output directories");
        dirs.add_options()
            ("gamedir", po::wvalue<wstring>(&gamedir)->value_name("dir"), "Directory of the game you want to extract files from.")
            ("outdir", po::wvalue<wstring>(&outdir)->value_name("dir"), "Output directory for extraced files.")
            ;
        po::options_description cmdline_options;
        cmdline_options.add(general).add(dirs);

        po::positional_options_description pos;
        pos.add("gamedir", 1).add("outdir", 1);

        po::variables_map vmap;
        po::store(po::wcommand_line_parser(argc, argv).options(cmdline_options).positional(pos).run(), vmap);

        if (vmap.count("help"))
        {
            print_help(general);
            exit(1);
        }

        vmap.notify();
        if (!vmap.count("gamedir"))
        {
            g_log.fatal("No input directory set\n");
            throw std::invalid_argument("No input directory set.\n");
        }
        if (!vmap.count("outdir"))
        {
            outdir = fs::absolute(fs::system_complete(argv[0]).parent_path()).generic_wstring();
            g_log.info("No output directory set. Using default: " + narrow(outdir) + '\n');
        }
        options.game_dir = narrow(gamedir);
        options.out_dir = narrow(outdir);
    }
    catch (const exception& e)
    {
        u8error(e.what());
        exit(1);
    }
}

void scan_pac_files(const fs::path& in_path, vector<fs::path>& pac_files) {
    auto data_dir = in_path;
    data_dir.append(L"data");
    
    if (!fs::exists(data_dir) || !fs::is_directory(data_dir))
    {
        g_log.fatal("Input directory does not contain \"data\" subdir.\n");
        exit(1);
    }

    g_log.info("Scanning \"data\" subdir for PAC files.\n");
    for (auto&& current : fs::directory_iterator(data_dir))
    {
        if (current.path().extension().compare(L".pac") == 0)
        {
            g_log.trace("Found file: " + narrow(current.path().generic_wstring()) + '\n');
            pac_files.push_back(current.path());
        }
    }
    int base_num = pac_files.size();
    {
        stringstream buf;
        buf << "Finished scanning \"data\" subdir. Found " << base_num << " PAC files.\n";
        g_log.trace(buf.str());
    }

    auto dlc_base_dir = in_path;
    dlc_base_dir.append(L"DLC");
    if (fs::exists(dlc_base_dir) && fs::is_directory(dlc_base_dir))
    {
        g_log.info("Scanning DLC subdir for PAC files\n");
        for (auto&& dlc : fs::directory_iterator(dlc_base_dir))
        {
            for (auto&& current : fs::directory_iterator(dlc))
            {
                if (current.path().extension().compare(L".pac") == 0) {
                    g_log.trace("Found file: " + narrow(current.path().generic_wstring()) + '\n');
                    pac_files.push_back(current.path());
                }
            }
        }
        {
            stringstream buf;
            buf << "Finished scanning \"DLC\" subdir. Found " << pac_files.size() - base_num << " PAC files\n";
            g_log.trace(buf.str());
        }
    } else
    {
        g_log.info("DLC directory not found. Skipping\n");
    }

    stringstream buf;
    buf << "Finished scanning PAC files. Found " << pac_files.size() << " files\n";
    g_log.info(buf.str());
}

void dump_csv(vector<pac> pacs)
{
    g_log.trace("Generating CVS data.\n");
    fs::ofstream pac_csv("pac.csv", ios_base::binary);
    fs::ofstream index_csv("index.csv", ios_base::binary);
    for (auto current : pacs)
    {
        g_log.trace("Current PAC file: " + current.get_filename() + '\n');
        current.header_csv(pac_csv);
        current.index_csv(index_csv);
    }
}

int wmain(int argc, wchar_t* argv[]) {
    g_log.set_logfile("nep.log");
    g_log.set_level(log::log_level::trace);

    
    // Process command line
    options options { "", "" };
    parse_commandline(argc, argv, options);

    fs::path in_path(widen(options.game_dir));
    g_log.trace("Input directory: " + narrow(in_path.generic_wstring()) + '\n');
    if (!exists(in_path))
    {
        g_log.fatal("Input directory doesn't exist.\n");
        exit(1);
    }
    if (!is_directory(in_path))
    {
        g_log.fatal("Input directory is not a directory.\n");
        exit(1);
    }
    vector<fs::path> pac_files;
    scan_pac_files(in_path, pac_files);

    vector<pac> pacs;
    pacs.reserve(pac_files.size());
    pac archive;
    for (auto current : pac_files)
    {
        archive.open(current.string());
        pacs.push_back(archive);
        archive.reset();
    }
    vector<filter> filters;
    filters.emplace_back(pac::ext_filters::gbin);
    filters.emplace_back(pac::ext_filters::cl3);
    for (pac current : pacs)
    {
        auto filename = fs::path(current.get_filename()).filename();
        if (filename.compare(string("SYSTEM00000.pac")) == 0)
        {
            current.extract(filters);
        }
    }
    //dump_csv(pacs);
    return 0;
}


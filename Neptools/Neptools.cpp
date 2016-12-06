// Neptools.cpp : Defines the entry point for the to_console application.
//

#include "stdafx.h"
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "SLog.hpp"
#include "PacArchive.h"
#include <codecvt>

using namespace neptools;
using namespace std;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

Log::Logger g_log;

struct Options
{
    wstring game_dir;
    wstring out_dir;
};


void print_help(po::options_description desc)
{
    cout << "\n\tUsage: neptools.exe <gamedir> <outdir>" << "\n\n";
    cout << "  <gamedir>\t\tDirectory of the game you want to extract files from." << '\n';
    cout << "  <outdir>\t\tOutput directory for extracted files." << "\n\n";
    cout << desc << endl;
}

//Parse the command line
//
void parse_commandline(int argc, wchar_t** argv, Options& options)
{
    try
    {
        po::options_description general("General options");
        general.add_options()
            ("help,h", "Display help message")
            ;
        po::options_description dirs("Input and output directories");
        dirs.add_options()
            ("gamedir", po::wvalue<wstring>(&options.game_dir)->value_name("dir"), "Directory of the game you want to extract files from.")
            ("outdir", po::wvalue<wstring>(&options.out_dir)->value_name("dir"), "Output directory for extraced files.")
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
            g_log.fatal(L"No input directory set\n");
            throw std::invalid_argument("No input directory set.");
        }
        if (!vmap.count("outdir"))
        {
            options.out_dir = fs::absolute(fs::system_complete(argv[0]).parent_path()).generic_wstring();
            g_log.info(L"No output directory set. Using default: " + options.out_dir + L'\n');
        }
    }
    catch (const exception& e)
    {
        wcout << e.what() << endl;
        exit(1);
    }
}

void scan_pac_files(fs::path in_path, vector<fs::path>& pac_files)
{

    auto data_dir = in_path;
    data_dir.append(L"data");
    g_log.trace(L"Current data path is: " + data_dir.wstring() + L'\n');

    if (!fs::exists(data_dir) || !fs::is_directory(data_dir))
    {
        g_log.fatal(L"Input directory does not contain \"data\" subdir.\n");
        exit(1);
    }

    g_log.info(L"Scanning \"data\" subdir for PAC files.\n");
    for (auto&& current : fs::directory_iterator(data_dir))
    {
        if (current.path().extension().compare(L".pac") == 0)
        {
            g_log.trace(L"Current file: " + current.path().wstring() + L'\n');
            pac_files.push_back(current.path());
        }
    }


    auto dlc_base_dir = in_path;
    dlc_base_dir.append(L"DLC");
    if (fs::exists(dlc_base_dir) && fs::is_directory(dlc_base_dir))
    {
        g_log.info(L"Scanning DLC subdir for PAC files\n");
        for (auto&& dlc : fs::directory_iterator(dlc_base_dir))
        {
            for (auto&& current : fs::directory_iterator(dlc))
            {
                if (current.path().extension().compare(L".pac") == 0) {
                    g_log.trace(L"Current files: " + current.path().wstring() + L'\n');
                    pac_files.push_back(current.path());
                }
            }
        }
    }
    else
    {
        g_log.info(L"DLC directory not found. Skipping\n");
    }

    wstringstream buffer;
    buffer << L"Finished scanning for PAC files. Found " << pac_files.size() << L" files.\n";
    g_log.info(buffer.str());
}

int wmain(int argc, wchar_t* argv[])
{
    // initialize locales
    //locale::global(locale("en_US.UTF-8"));
    wcout.imbue(locale(""));

    //g_log.set_logfile(L"nep.log");
    g_log.set_level(Log::LogLevel::Trace);

    // Process command line
    Options options{ L"", L"" };
    parse_commandline(argc, argv, options);

    fs::path in_path(options.game_dir);
    g_log.trace(L"Input directory: " + in_path.wstring() + L'\n');
    if (!exists(in_path))
    {
        g_log.fatal(L"Input directory doesn't exist.\n");
        exit(1);
    }
    if (!is_directory(in_path))
    {
        g_log.fatal(L"Input directory is not a directory.\n");
        exit(1);
    }
    vector<fs::path> pac_files;
    scan_pac_files(in_path, pac_files);
    
    vector<PacArchive> pacs;
    pacs.reserve(pac_files.size());
    PacArchive arc;
    for (auto current : pac_files)
    {
        arc.open(current.wstring());
        pacs.push_back(arc);
        arc.reset();
    }

    g_log.trace(L"Generating CVS data.\n");
    fs::ofstream pac_csv("pac.csv");
    fs::ofstream index_csv("index.csv");
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    for (auto current : pacs)
    {
        g_log.trace(L"Current PAC file: " + current.get_filename() + L'\n');
        pac_csv << converter.to_bytes(current.get_header_csv());
        index_csv << converter.to_bytes(current.get_index_csv());
    }
    return 0;
}


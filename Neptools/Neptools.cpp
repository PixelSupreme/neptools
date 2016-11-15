// Neptools.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <boost/program_options.hpp>

#include "PacArchive.h"

using namespace neptools;
using namespace std;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

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
bool get_commandline(int argc, wchar_t** argv, Options options)
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
            return false;
        }

        vmap.notify();
        if (!vmap.count("gamedir"))
        {
            cout << "Error: no input directory specified\n";
            print_help(general);
            return false;
        }
        if (!vmap.count("outdir"))
        {
            cout << "Error: no output directory specified\n";
            print_help(general);
            return false;
        }
        return true;
    }
    catch (const exception& e)
    {
        wcout << e.what() << endl;
        return false;
    }
}

int wmain(int argc, wchar_t* argv[])
{
    // initialize char conversion from system locale
    locale::global(locale("")); 

    // Process command line
    Options options{L"blah", L"blubb"};

    {
        if (!get_commandline(argc, argv, options))
        {
            return -1;
        }
    }

    return 0;
}


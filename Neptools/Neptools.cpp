// Neptools.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

#include "PacArchive.h"

using namespace neptools;

namespace fs = boost::filesystem;

int main()
{
    std::string file_name{ "c:\\test\\game00001.pac" };
    PacArchive archive{ fs::path(file_name) };
    archive.open();
    return 0;
}


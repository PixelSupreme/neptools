// strtools.hpp
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
//
// Purpose : Utility functions for working with UTF-8 encoded strings
//      as basic string representaion. Provides conversions between
//      UTF-8 and UTF-16 and UTF-8 and ANSI codepages.

#pragma once

#include "stdafx.h"

#include "strtools.h"

#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Convert from UTF-8 to UTF-16
std::wstring widen(const std::string& in) {
    auto in_size = in.size();
    auto wide_size = ::MultiByteToWideChar(CP_UTF8, 0, &in[0], in_size, nullptr, 0);
    std::wstring out(wide_size, 0);
    ::MultiByteToWideChar(CP_UTF8, 0, &in[0], in_size, &out[0], wide_size);
    return out;

}

// Conver from UTF-16 to UTF-8
std::string narrow(const std::wstring& in) {
    auto in_size = in.size();
    auto narrow_size = ::WideCharToMultiByte(CP_UTF8, 0, &in[0], in_size, nullptr, 0, nullptr, nullptr);
    std::string out(narrow_size, 0);
    ::WideCharToMultiByte(CP_UTF8, 0, &in[0], in_size, &out[0], narrow_size, nullptr, nullptr);
    return out;
}

// Print UTF-8 encoded string to stdout.
void u8print(const std::string& in) {
    auto buffer = widen(in);
    ::WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), buffer.c_str(), buffer.length(), nullptr, nullptr);
}

// Print UTF-8 encoded string to stderr.
void u8error(const std::string& in) {
    auto buffer = widen(in);
    ::WriteConsoleW(GetStdHandle(STD_ERROR_HANDLE), buffer.c_str(), buffer.length(), nullptr, nullptr);
}

// Print UTF-16 encoded string to stdout
void u16print(const std::wstring& in)
{
    ::WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), in.c_str(), in.length(), nullptr, nullptr);
}

// Print UTF-16 encoded string to stderr
void u16error(const std::wstring& in) {
    ::WriteConsoleW(GetStdHandle(STD_ERROR_HANDLE), in.c_str(), in.length(), nullptr, nullptr);
}



// Convert from ANSI codepage to UTF-8
std::string cp2utf(const std::string& in, const unsigned int cp) {
    if (in.empty())
    {
        return std::string();
    }
    // Codepage to UTF16
    auto wide_size = ::MultiByteToWideChar(cp, 0, &in[0], in.size(), nullptr, 0);
    std::wstring buffer(wide_size, 0);
    ::MultiByteToWideChar(cp, 0, &in[0], in.size(), &buffer[0], wide_size);
    return narrow(buffer);
}

// Convert from UTF-8 to ANSI codepage
std::string utf2cp(const std::string& in, const unsigned int cp) {
    if (in.empty())
    {
        return std::string();
    }
    auto buffer = widen(in);
    // UTF16 to codepage
    auto narrow_size = ::WideCharToMultiByte(cp, 0, &buffer[0], buffer.size(), nullptr, 0, nullptr, nullptr);
    std::string out(narrow_size, 0);
    ::WideCharToMultiByte(cp, 0, &buffer[0], buffer.size(), &out[0], narrow_size, nullptr, nullptr);
    return out;
}


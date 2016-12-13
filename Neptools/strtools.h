// strtools.hpp
//
// Purpose : Utility functions for working with UTF-8 encoded strings
//      as basic string representaion. Provides conversions between
//      UTF-8 and UTF-16 and UTF-8 and ANSI codepages.

#pragma once

#include <string>

// Convert from UTF-8 to UTF-16
std::wstring widen(const std::string& in);

// Conver from UTF-16 to UTF-8
std::string narrow(const std::wstring& in);

// Print UTF-8 encoded string to stdout.
void u8print(const std::string& in);

// Print UTF-8 encoded string to stderr.
void u8error(const std::string& in);

// Print UTF-16 encoded string to stdout
void u16print(const std::wstring& in);

// Print UTF-16 encoded string to stderr
void u16error(const std::wstring& in);

// Convert from ANSI codepage to UTF-8
std::string cp2utf(const std::string& in, const unsigned int cp);

// Convert from UTF-8 to ANSI codepage
std::string utf2cp(const std::string& in, const unsigned int cp);

// Convert from UTF-8 to ANSI codepage
std::string utf2cp(const std::string& in, const unsigned int cp);

// Convert from Shift_JIS to UTF-8
inline std::string shift_jis2utf(const std::string& in) {
    return cp2utf(in, 932);
}

// Convert from UTF-8 to Shift_JIS
inline std::string utf2shift_jis(const std::string& in) {
    return utf2cp(in, 932);
}

#pragma once

#define _WIN32_WINNT 0x0600
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

namespace neptools
{
    std::wstring shift_jis2utf16(std::string in)
    {
        if (in.empty())
        {
            return std::wstring();
        }
        int wide_size = ::MultiByteToWideChar(932, 0, &in[0], static_cast<int>(in.size()), nullptr, 0);
        std::wstring out(wide_size, 0);
        ::MultiByteToWideChar(932, 0, &in[0], static_cast<int>(in.size()), &out[0], wide_size);
        return out;
    }
}

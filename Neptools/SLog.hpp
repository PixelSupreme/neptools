// SLog.h
//
// Project: Neptools
// Author: PixelSupreme
//
// Simpley logging facility

#pragma once

#include <atomic>
#include <chrono>
#include <iomanip>

using namespace std;

namespace neptools {
namespace Log
{
enum class LogLevel { Trace, Info, Warning, Error, Fatal };

class Logger
{
private:
    LogLevel log_level;
    bool to_console;
    wofstream logfile;

public:
    Logger() :
        log_level{ LogLevel::Error },
        to_console{ true }
    {

    }

    Logger(const Logger&) = delete;

    Logger(Logger&&) = default;

    Logger& operator=(const Logger&) = delete;

    Logger& operator=(Logger&&) = default;

    ~Logger()
    {
        logfile.close();
    };

    void set_level(LogLevel level) noexcept {
        log_level = level;
    }

    bool check_level(LogLevel level) const noexcept {
        return level >= log_level;
    }

    void log_to_con(bool mode) noexcept {
        to_console = mode;
    }

    void set_logfile(const wstring& filename)
    {
        logfile = wofstream(filename);
        if (!logfile.is_open())
        {
            wcerr << L"[ERR] Failed to open logfile " << filename << endl;
        }
    }

    void close_logfile()
    {
        logfile.close();
    }

    void trace(const wstring& message)
    {
        if (log_level <= LogLevel::Trace) {
            write(L"[TRC] " + message);
        }
    }

    void info(const wstring& message)
    {
        if (log_level <= LogLevel::Info) {
            write(L"[IFO] " + message);
        }
    }

    void warning(const wstring& message)
    {
        if (log_level <= LogLevel::Warning) {
            write(L"[WRN] " + message);
        }
    }

    void error(const wstring& message)
    {
        if (log_level <= LogLevel::Error) {
            write(L"[ERR] " + message);
        }
    }

    void fatal(const wstring& message)
    {
        if (log_level <= LogLevel::Fatal) {
            write(L"[FAT] " + message);
        }
    }

private:
    void write(const wstring& message)
    {
        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        struct tm time;
        _localtime64_s(&time, &now);
        wstringstream timestamp; 
        timestamp << put_time(&time, L"%Y-%m-%d %X: ");
        if (logfile.is_open())
        {
            logfile <<  timestamp.str() << message;
        }
        if (to_console)
        {
            wcout << timestamp.str() << message;
        }
    }
};

}
}
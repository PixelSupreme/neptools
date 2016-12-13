// SLog.h
//
// Project: Neptools
// Author: PixelSupreme
//
// Simpley logging facility

#pragma once

#include "strtools.h"

#include <chrono>
#include <iomanip>

using namespace std;

namespace neptools {
namespace log
{
enum class log_level { trace, info, warning, error, fatal };

class logger
{
private:
    log_level level;
    bool to_console;
    ofstream logfile;

public:
    logger() :
        level{ log_level::error },
        to_console{ true }
    {

    }

    logger(const logger&) = delete;

    logger(logger&&) = default;

    logger& operator=(const logger&) = delete;

    logger& operator=(logger&&) = default;

    ~logger()
    {
        logfile.close();
    };

    void set_level(log_level level) noexcept {
        this->level = level;
    }

    bool check_level(log_level level) const noexcept {
        return level >= this->level;
    }

    void log_to_con(bool mode) noexcept {
        to_console = mode;
    }

    void set_logfile(const string& filename)
    {
        logfile = ofstream(filename);
        if (!logfile.is_open())
        {
            u8error("[ERR] Failed to open logfile " + filename + '\n');
        }
    }

    void close_logfile()
    {
        logfile.close();
    }

    void trace(const string& message)
    {
        if (check_level(log_level::trace)) {
            write("[TRC] ", message);
        }
    }

    void info(const string& message)
    {
        if (check_level(log_level::info)) {
            write("[IFO] ", message);
        }
    }

    void warning(const string& message)
    {
        if (check_level(log_level::warning)) {
            write("[WRN] ", message);
        }
    }

    void error(const string& message)
    {
        if (check_level(log_level::error)) {
            write("[ERR] ", message);
        }
    }

    void fatal(const string& message)
    {
        if (check_level(log_level::fatal)) {
            write("[FAT] ", message);
        }
    }

private:
    void write(const string& prefix, const string& message)
    {
        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        struct tm time;
        _localtime64_s(&time, &now);
        stringstream buff;
        buff << put_time(&time, "%Y-%m-%d %X: ") << prefix << message;
        string log_line = buff.str();
        if (logfile.is_open())
        {
            logfile << log_line;
        }
        if (to_console)
        {
            u8print(log_line);
        }
    }
};

}
}
#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <mutex>

class log
{
public:
    enum Level
    {
        DEBUG = 0,
        INFO,
        WARN,
        ERROR
    };

    static const char* Level_s[4];

public:
    static log* insetance();

public:
    const bool open(std::string argv_log_name);
    void close();

    void set_log_path(std::string argv_log_path);
    void set_read_Level(Level argv_Level);
    void set_log_max_size(std::size_t argv_log_max_size);

    const bool read_log(Level argv_log_Level, std::string argv_log_name, int argv_file_line, const char* argv_log_str);
    const bool DEBUG_read_log(std::string argv_log_name, int argv_file_line, const char* argv_log_str);
    const bool INFO_read_log(std::string argv_log_name, int argv_file_line, const char* argv_log_str);
    const bool WARN_read_log(std::string argv_log_name, int argv_file_line, const char* argv_log_str);
    const bool ERROR_read_log(std::string argv_log_name, int argv_file_line, const char* argv_log_str);


private:
    void new_log_file();


private:
    log();
    ~log();

private:
    static log * _log;
    Level _read_Leve;

    std::string log_path;
    std::string log_name;
    std::ofstream _file;
    int file_ptr;
    char timestr[32];

    std::size_t log_max_size;

private:
    std::mutex _read_file_mutex;
};





#pragma once
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string.h>

class file_io
{
public:
    file_io() = delete;
    file_io(std::string argv_file_name, std::ios_base::openmode  argv_file_open_type, int argv_file_read_size);
    ~file_io();

public:
    const bool file_is_open();
    const char* get_file_str();
    const bool file_EOF();
    const int file_size();

public:
    int set_file_str(std::string argv_write_str); 
    int ser_file_str(const char* argv_write_str, std::size_t argv_write_str_size);

private:
    std::string file_name;
    std::ios::openmode file_open_type;
    int file_read_size;

    std::fstream _file;
    int _file_size;
    char* file_str;
};




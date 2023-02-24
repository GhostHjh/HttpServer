#include "file_io.h"
#include <cstddef>
#include <cstring>
#include <iterator>
#include <string>

file_io::file_io(std::string argv_file_name, std::ios::openmode argv_file_open_type, int argv_file_read_size)
    : file_name(argv_file_name), file_open_type(argv_file_open_type), file_read_size(argv_file_read_size), _file_size(0), file_str(nullptr)
{
    _file.open(argv_file_name.c_str(), file_open_type);

    if (_file.is_open())
    {
        _file.seekg(0, std::ios::end);
        _file_size = _file.tellg();
        _file.seekg(0, std::ios::beg);
    }

    std::cout << "文件大小 " << _file_size << " bit\n";

    file_str = new char[file_read_size];
}

file_io::~file_io()
{
    if (_file.is_open())
        _file.close();
    if (file_str != nullptr)    
        delete[](file_str);
}

const bool file_io::file_is_open()
{
    return _file.is_open();
}

const char* file_io::get_file_str()
{
    if (!_file.is_open())
        return nullptr;
    
    memset(file_str, 0, static_cast<size_t>(file_read_size));
    //_file.get(file_str, static_cast<std::streamsize>(file_read_size));
    _file.read(file_str, static_cast<std::streamsize>(file_read_size));

    return file_str;
}

const bool file_io::file_EOF()
{
    if (!_file.is_open())
        return true;

    return _file.eof();
}

const int file_io::file_size()
{
    return _file_size;    
}

//***********************************************************************************
int file_io::set_file_str(std::string argv_write_str)
{
    if (!_file.is_open())
        return 0;

    _file.write(argv_write_str.c_str(), static_cast<std::streamsize>(argv_write_str.size()));

    if (_file.bad())
        return 0;
    
    return argv_write_str.size();
}

int file_io::ser_file_str(const char *argv_write_str, std::size_t argv_write_str_size)
{
    if (!_file.is_open())
        return 0;

    _file.write(argv_write_str, static_cast<std::streamsize>(argv_write_str_size));

    if (_file.bad())
        return 0;
    
    return static_cast<int>(argv_write_str_size);
}
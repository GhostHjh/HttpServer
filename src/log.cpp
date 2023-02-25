#include "log.h"
#include <fstream>
#include <mutex>
#include <stdexcept>
#include <string.h>
#include <string>
#include <time.h>

const char* log::Level_s[4] = {"DEBUG", "INFO", "WARN", "ERROR"};

log* log::_log = new log;

log::log() :_read_Leve(log::DEBUG), log_path("./"), log_max_size(1024*1024*10) {}
log::~log() {}

log* log::insetance()
{
    //if (log::_log == nullptr)
    //    log::_log = new log();
    
    return log::_log;
}

void log::close()
{
    if (log::_log != nullptr)
        delete log::_log;
    
    if (_file.is_open())
        _file.close();
}

const bool log::open(std::string argv_log_name)
{
    if (log_path[log_path.size() -1] != '/')
        log_path += '/';
    
    log_name = argv_log_name;

    _file.open((log_path + log_name).c_str(), std::ios::app);

    return _file.is_open();
}

void log::set_read_Level(Level argv_Level)
{
    log::_read_Leve = argv_Level;
}

void log::set_log_max_size(std::size_t argv_log_max_size)
{
    log::log_max_size = argv_log_max_size;
}

const bool log::read_log(Level argv_log_Level, std::string argv_log_name, int argv_file_line, const char *argv_log_str)
{
    {
        std::unique_lock<std::mutex> _mutex_lock(log::_read_file_mutex);

        if (!_file.is_open())
            return false;
        else if (argv_log_Level < log::_read_Leve)
            return true;
    
        time_t _time = time(nullptr);
        tm* _tm = localtime(&_time);
        memset(log::timestr, 0, 32);
        strftime(log::timestr, 32, "%Y-%m-%d %H:%M:%S", _tm);
        //std::cout << log::timestr << std::endl;
    
    
        _file << '[' << log::Level_s[argv_log_Level] << ']' << ' '
            << '(' << log::timestr << ')' << ' '
            << '{' << argv_log_name << ' ' << argv_file_line << '}' << ' '
            << argv_log_str << "\r\n";
        //std::cout << '[' << log::Level_s[argv_log_Level] << ']' << ' '
        //    << '(' << log::timestr << ')' << ' '
        //    << '{' << argv_log_name << ' ' << argv_file_line << '}' << ' '
        //    << argv_log_str << "\r\n";
        _file.flush();
    }

    new_log_file();

    return true;
}

const bool log::DEBUG_read_log(std::string argv_log_name, int argv_file_line, const char* argv_log_str)
{
    return read_log(log::DEBUG, argv_log_name, argv_file_line, argv_log_str);
}
const bool log::INFO_read_log(std::string argv_log_name, int argv_file_line, const char* argv_log_str)
{
    return read_log(log::INFO, argv_log_name, argv_file_line, argv_log_str);
}
const bool log::WARN_read_log(std::string argv_log_name, int argv_file_line, const char* argv_log_str)
{
    return read_log(log::WARN, argv_log_name, argv_file_line, argv_log_str);
}
const bool log::ERROR_read_log(std::string argv_log_name, int argv_file_line, const char* argv_log_str)
{
    return read_log(log::ERROR, argv_log_name, argv_file_line, argv_log_str);
}


//********************************************************************************************************
void log::new_log_file()
{
    {
        std::unique_lock<std::mutex> _lock(log::_read_file_mutex);
        //_file.seekp(0, std::ios::beg);
        //_file.seekp(0, std::ios::end);

        log::file_ptr = _file.tellp();

        if (log::file_ptr< log::log_max_size)
        {
            //std::cout << "文件当前大小"<< tmp_file_size << "不用更新文件\n";
            return;
        }
        //else
        //{
        //    std::cout << "文件当前大小"<< log::file_ptr << "需要更新文件\n";  
        //}
        _file.close();

        time_t _time = time(nullptr);
        tm* _tm = localtime(&_time);
        memset(log::timestr, 0, 32);
        strftime(log::timestr, 32, "%Y-%m-%d_%H:%M:%S", _tm);

        std::ifstream tmp_file_i( std::string( log::log_path + log::log_name ).c_str()) ;
        std::ofstream tmp_file_o( std::string( log::log_path + log::timestr + '-' + log::log_name).c_str() , std::ios::app);

        if (!tmp_file_i.is_open() || !tmp_file_o.is_open())
        {
            tmp_file_i.close();
            tmp_file_o.close();
            std::logic_error("new log file error!!!");
            return;
        }

        char* tmp_buff = new char[1024*1024*10];
        while (!tmp_file_i.eof())
        {
            memset(tmp_buff, 0, 1024*1024*10);
            tmp_file_i.read(tmp_buff, 1024*1024*10);
            tmp_file_o << tmp_buff;
        }
        
        tmp_file_i.close();
        tmp_file_o.close();

        _file.open((log_path + log_name).c_str(), std::ios::trunc);
    }

}



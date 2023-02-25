#include "http_server.h"
#include "unistd.h"
#include <functional>
#include <ios>
#include <string>
#include <sys/socket.h>
#include <thread>
#include "signal.h"

http_server::http_server()
    : _epoll_server(nullptr)
{

}

http_server::~http_server()
{
    if (_epoll_server == nullptr)
        delete(_epoll_server);
}

void http_server::start()
{
    if (_config._web_file_path[_config._web_file_path.size() -1] == '/')
        _config._web_file_path.pop_back();
    
    //_epoll_server->set_client_func(std::move( std::bind(&http_server::client_read_write, this, std::placeholders::_1, std::placeholders::_2)));
    _epoll_server->set_client_func(std::bind(&http_server::client_read_write, this, std::placeholders::_1));
    //_epoll_server->set_client_func(&l_client_read_write);
    _epoll_server->status();
}

void http_server::set_config(std::string argv_ip, int argv_port, std::string argv_web_file_path, std::string argv_web_default, std::string argv_log_path, std::string argv_long_file_name)
{
    _config._IP = argv_ip;
    _config._PORT = argv_port;
    _config._web_file_path = argv_web_file_path;
    _config._web_default = argv_web_default;
    _config._log_path = argv_log_path;
    _config._log_file_name = argv_long_file_name;

    //log::insetance()->set_log_path(_config._log_path);
    //log::insetance()->open(_config._log_file_name);

    if (_epoll_server != nullptr)
        delete(_epoll_server);
    
    _epoll_server = new epoll_server(argv_ip, argv_port);
}

//void http_server::client_read_write(int argv_client_fd, epoll_server& argv_this)
//{
//    int& client_socket = argv_client_fd;
//
//    char* buff = new char[1024];
//    std::string buff_plus;
//
//    int read_size = read(client_socket, buff, 1024);
//
//    if (read_size == 0)
//    {
//        delete[](buff);
//        close(client_socket);
//        argv_this.DEL_epoll_evs(client_socket);
//        return;
//    }
//
//    buff_plus += buff;
//    for (; read_size == 1024; )
//    {
//        memset(buff, 0, 1024);
//        read_size = read(client_socket, buff, 1024);
//
//        //cout << read_size << endl;
//        buff_plus += buff;
//    }
//    delete[](buff);
//
//    //cout << buff_plus << endl;
//    web_file_read(client_socket, buff_plus);
//
//    close(client_socket);
//    argv_this.DEL_epoll_evs(client_socket);
//    std::cout << "发送数据程序结束\n";
//}

//void http_server::client_read_write(int argv_client_fd)
//{
//    int& client_socket = argv_client_fd;
//
//    char* buff = new char[1024];
//    std::string buff_plus;
//
//    int read_size = read(client_socket, buff, 1024);
//
//    if (read_size < 1)
//    {
//        close(client_socket);
//        return;
//    }
//
//    buff_plus += buff;
//    for (; read_size == 1024; )
//    {
//        memset(buff, 0, 1024);
//        read_size = read(client_socket, buff, 1024);
//
//        //cout << read_size << endl;
//        buff_plus += buff;
//    }
//    delete[](buff);
//
//    //cout << buff_plus << endl;
//    web_file_read(client_socket, buff_plus);
//
//    close(client_socket);
//    std::cout << "发送数据程序结束\n\n\n";
//}
//
//void http_server::web_file_read(int& argv_client_socket, std::string& argv_client_header)
//{
//
//    http_header tmp_header(argv_client_header, _config._web_default);
//    if (tmp_header.client_header_is_ok())
//    {
//        std::cout << "请求路径" << _config._web_file_path << tmp_header.get_request_path() << std::endl; 
//        file_io _file(_config._web_file_path + tmp_header.get_request_path(), file_read_b(tmp_header.get_accept_type()), 1024);
//        if (!_file.file_is_open())
//            return;
//        
//        tmp_header.add_server_header_request_status();
//        tmp_header.add_server_header_request_type_length(tmp_header.get_accept_type(), _file.file_size());
//        tmp_header.add_serverheader_request_end();
//
//        write(argv_client_socket, tmp_header.get_server_header().c_str(), tmp_header.get_server_header().size());
//        write(argv_client_socket, _file.get_file_str(), 1024);
//    
//        for (; !_file.file_EOF();)
//            write(argv_client_socket, _file.get_file_str(), 1024);
//    }
//}
//
//const std::ios_base::openmode http_server::file_read_b(const std::string& argv_file_type)
//{
//    if (argv_file_type.size() == 0)
//        return std::ios::in;
//    else if (argv_file_type[0] == 't' && argv_file_type[1] == 'e' && argv_file_type[2] == 'x' && argv_file_type[3] == 't')
//        return std::ios::in;
//    else if (argv_file_type == "*/*")
//        return std::ios::in;
//    
//    return std::ios::in | std::ios::binary;
//}

void http_server::client_read_write(int argv_client_fd)
{
    signal(SIGPIPE, SIG_IGN);
    int& client_socket = argv_client_fd;

    char* buff = new char[1024];
    std::string client_header;

    std::cout << "开始接收数据\n";
    int read_size = read(client_socket, buff, 1024);

    if (read_size < 1)
    {
        std::cout << "客户端断开链接\n\n\n";
        close(client_socket);
        return;
    }

    client_header += buff;
    for (; read_size == 1024; )
    {
        memset(buff, 0, 1024);
        read_size = read(client_socket, buff, 1024);

        //cout << read_size << endl;
        client_header += buff;
    }
    delete[](buff);
    std::cout << "数据全部接收完成\n";
    //cout << buff_plus << endl;
    //web_file_read(client_socket, buff_plus);


    http_header tmp_header(client_header, _config._web_default);
    if (tmp_header.client_header_is_ok())
    {
        std::cout << "请求路径" << _config._web_file_path << tmp_header.get_request_path() << std::endl;

        std::ios_base::openmode file_opmode = std::ios::in;
        if (tmp_header.get_accept_type().size() == 0)
            file_opmode = std::ios::in;
        else if (tmp_header.get_accept_type()[0] == 't' && tmp_header.get_accept_type()[1] == 'e' && tmp_header.get_accept_type()[2] == 'x' && tmp_header.get_accept_type()[3] == 't')
            file_opmode = std::ios::in;
        else if (tmp_header.get_accept_type() == "*/*")
            file_opmode = std::ios::in;
        else
            file_opmode = std::ios::in | std::ios::binary;

        file_io _file(_config._web_file_path + tmp_header.get_request_path(), file_opmode, 102400);
        if (!_file.file_is_open())
        {
            std::cout << "没有客户端要的资源，直接断开链接\n\n\n";
            close(client_socket);
            return;
        }
        
        tmp_header.add_server_header_request_status();
        tmp_header.add_server_header_request_type_length(tmp_header.get_accept_type(), _file.file_size());
        tmp_header.add_serverheader_request_end();

        int write_size = write(client_socket, tmp_header.get_server_header().c_str(), tmp_header.get_server_header().size());
        for (; !_file.file_EOF() && write_size > 0;)
        {
            write_size = write(client_socket, _file.get_file_str(), 102400);
        }
        
        if (write_size < 1)
            std::cout << "客户端主动断开一次\n";
    }

    close(client_socket);
    std::cout << "断开客户端socket链接\n\n\n";
}



#include "http_server.h"
#include "epoll_server.h"
#include "unistd.h"
#include <functional>
#include <ios>
#include <string>
#include <sys/socket.h>
#include <thread>
#include "signal.h"

http_server::http_server()
{

}

http_server::~http_server()
{
    epoll_server::GET()->stop();
}

void http_server::start()
{
    if (_config._web_fpath[_config._web_fpath.size() -1] == '/')
        _config._web_fpath.pop_back();
    
    //_epoll_server->set_client_func(std::move( std::bind(&http_server::client_read_write, this, std::placeholders::_1, std::placeholders::_2)));
    //_epoll_server->set_client_func(std::bind(&http_server::client_read_write, this, std::placeholders::_1));
    //_epoll_server->set_client_func(&l_client_read_write);
    //_epoll_server->status();

    epoll_server::GET()->set_client_func(std::bind(&http_server::client_read_write, this, std::placeholders::_1));
    epoll_server::GET()->status();
}

void http_server::set_config(std::string argv_ip, int argv_port, std::string argv_web_fpath, std::string argv_web_default_index_fname, std::string argv_log_path, std::string argv_long_file_name)
{
    _config._IP = argv_ip;
    _config._PORT = argv_port;
    _config._web_fpath = argv_web_fpath;
    _config._web_default_index_fname = argv_web_default_index_fname;
    _config._log_path = argv_log_path;
    _config._log_fname = argv_long_file_name;

    if (_config._web_fpath[_config._web_fpath.size() -1] == '/')
        _config._web_fpath.pop_back();
    
    if (_config._web_default_index_fname[0] != '/')
        _config._web_default_index_fname = '/' + _config._web_default_index_fname;

    //log::insetance()->set_log_path(_config._log_path);
    //log::insetance()->open(_config._log_fname);

    epoll_server::GET()->config(_config._IP, _config._PORT);
}

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

    web_file_read(client_socket, client_header);

    epoll_server::GET()->DEL_epoll_evs(client_socket);
    close(client_socket);
    std::cout << "断开客户端socket链接\n\n\n";
}

void http_server::web_file_read(int& argv_client_socket, std::string& argv_client_header)
{
    http_header tmp_header(argv_client_header);
    if (tmp_header.client_header_is_ok())
    {
        std::cout << "请求路径" << tmp_header.get_request_path() << std::endl;

        //检测请求的文件类型
        std::ios_base::openmode file_opmode = std::ios::in;
        if (tmp_header.get_accept_type().size() == 0)
            file_opmode = std::ios::in;
        else if (tmp_header.get_accept_type()[0] == 't' && tmp_header.get_accept_type()[1] == 'e' && tmp_header.get_accept_type()[2] == 'x' && tmp_header.get_accept_type()[3] == 't')
            file_opmode = std::ios::in;
        else if (tmp_header.get_accept_type() == "*/*")
            file_opmode = std::ios::in;
        else
            file_opmode = std::ios::in | std::ios::binary;
        
        //设置要打开的文件
        std::string tmp_file_path_name = _config._web_fpath;

        if (tmp_header.get_request_path() == "/")
            tmp_file_path_name += _config._web_default_index_fname;
        else
            tmp_file_path_name += tmp_header.get_request_path();
    
        std::cout << "要打开的文件为" << tmp_file_path_name << std::endl;
        file_io _file(tmp_file_path_name, file_opmode, 102400);
        if (!_file.file_is_open())
        {
            tmp_header.add_server_header_request_status(1.1, 404, "Not Fount");
            tmp_header.add_serverheader_request_end();
            write(argv_client_socket, tmp_header.get_server_header().c_str(), tmp_header.get_server_header().size());
            std::cout << "没有客户端要的资源，直接断开链接\n\n\n";
            //close(argv_client_socket);
            return;
        }
        
        tmp_header.add_server_header_request_status();
        tmp_header.add_server_header_request_type_length(tmp_header.get_accept_type(), _file.file_size());
        tmp_header.add_serverheader_request_end();

        int write_size = write(argv_client_socket, tmp_header.get_server_header().c_str(), tmp_header.get_server_header().size());
        for (; !_file.file_EOF() && write_size > 0;)
        {
            write_size = write(argv_client_socket, _file.get_file_str(), 102400);
        }
        
        if (write_size < 1)
            std::cout << "客户端主动断开\n";
        else
            std::cout << "客户端请求内容发送完成\n";
    }
}





#include "http_server.h"
#include "unistd.h"
#include <functional>
#include <ios>
#include <string>
#include <signal.h>

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
    
    //_epoll_server->set_client_func(std::bind(&http_server::client_read_write, this, std::placeholders::_1, std::placeholders::_2));
    _epoll_server->set_client_func(std::bind(&http_server::client_read_write, this, std::placeholders::_1));
    _epoll_server->start();
}

void http_server::set_config(std::string argv_ip, int argv_port, std::string argv_web_file_path, std::string argv_web_default)
{
    _config._IP = argv_ip;
    _config._PORT = argv_port;
    _config._web_file_path = argv_web_file_path;
    _config._web_default = argv_web_default;

    if (_epoll_server != nullptr)
        delete(_epoll_server);
    
    _epoll_server = new epoll_server(argv_ip, argv_port);
}

//void http_server::client_read_write(epoll_event argv_ep_ev, epoll_server& argv_this)
//{
//    signal(SIGPIPE, SIG_IGN);
//    int client_socket = argv_ep_ev.data.fd;
//
//    char* buff = new char[10240];
//    
//    int read_size = read(client_socket, buff, 10240);
//    if (read_size > 0)
//    {
//        std::string client_header(buff);
//        for (; read_size == 10240; )
//        {
//            memset(buff, 0, 1024);
//            read_size = read(client_socket, buff, 10240);
//            //cout << read_size << endl;
//            client_header += buff;
//        }
//
//        //cout << buff_plus << endl;
//        web_file_read(client_socket, client_header);
//    }
//
//    delete[](buff);
//    argv_this.DEL_epoll_evs(client_socket);
//    close(client_socket);
//    std::cout << "发送数据程序结束\n";
//    return;
//}
void http_server::client_read_write(int argv_client_socket_fd)
{
    signal(SIGPIPE, SIG_IGN);
    int& client_socket = argv_client_socket_fd;

    char* buff = new char[10240];
    int read_size = read(client_socket, buff, 10240);
    if (read_size > 0)
    {
        std::string client_header(buff);
        for (; read_size == 10240; )
        {
            memset(buff, 0, 10240);
            read_size = read(client_socket, buff, 10240);
            //cout << read_size << endl;
            client_header += buff;
        }

        web_file_read(client_socket, client_header);
    }

    delete[](buff);
    epoll_server::DEL_epoll_evs(client_socket);
    close(client_socket);
    std::cout << "发送数据程序结束\n";
    return;
}

void http_server::web_file_read(int& argv_client_socket, std::string& argv_client_header)
{

    http_header tmp_header(argv_client_header, _config._web_default);
    if (tmp_header.client_header_is_ok())
    {
        std::cout << "请求路径" << _config._web_file_path << tmp_header.get_request_path() << std::endl;

        std::ios_base::openmode file_openmod;
        const std::string& _type = tmp_header.get_accept_type();
        if (_type.size() == 3 && _type[0] == '*' && _type[1] == '/' && _type[2] == '*')
            file_openmod = std::ios::in;
        else if (_type[0] == 't' && _type[1] == 'e' && _type[2] == 'x' && _type[3] == 't')
            file_openmod = std::ios::in;
        else if (_type.size() == 0)
            file_openmod = std::ios::in;
        else
            file_openmod = std::ios::in | std::ios::binary;

        file_io _file(_config._web_file_path + tmp_header.get_request_path(), file_openmod, 102400);
        if (!_file.file_is_open())
        {
            tmp_header.add_server_header_request_status(1.1, 404, "Not Found");
            tmp_header.add_serverheader_request_end();
            write(argv_client_socket, tmp_header.get_server_header().c_str(), tmp_header.get_server_header().size());
            return;
        } 

        
        tmp_header.add_server_header_request_status();
        tmp_header.add_server_header_request_type_length(tmp_header.get_accept_type(), _file.file_size());
        tmp_header.add_serverheader_request_end();

        int write_size = write(argv_client_socket, tmp_header.get_server_header().c_str(), tmp_header.get_server_header().size());

        for (; !_file.file_EOF() && write_size > 0;)
            write(argv_client_socket, _file.get_file_str(), 102400);    
    }
    return;
}



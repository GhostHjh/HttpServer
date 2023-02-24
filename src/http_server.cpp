#include "http_server.h"
#include "unistd.h"
#include <functional>
#include <string>

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
    
    _epoll_server->set_client_func(std::bind(&http_server::client_read_write, this, std::placeholders::_1, std::placeholders::_2));
    //_epoll_server->set_client_func(&l_client_read_write);
    _epoll_server->status();
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

void http_server::client_read_write(epoll_event argv_ep_ev, epoll_server& argv_this)
{
    int client_socket = argv_ep_ev.data.fd;

    char* buff = new char[1024];
    std::string buff_plus;

    for (int read_size = 1024; read_size == 1024; )
    {
        memset(buff, 0, 1024);
        read_size = read(client_socket, buff, 1024);
        //cout << read_size << endl;
        buff_plus += buff;
    }
    delete[](buff);

    //cout << buff_plus << endl;
    web_file_read(client_socket, buff_plus);
    //std::string& argv_client_header = buff_plus;
    //http_header tmp_header(argv_client_header);
    //if (tmp_header.client_header_is_ok())
    //{
    //    std::cout << "请求路径" << tmp_header.get_request_path() << std::endl;
    //    file_io _file(_config._web_file_path + tmp_header.get_request_path(), std::ios::in, 1024);
    //    tmp_header.add_server_header_request_status();
    //    tmp_header.add_server_header_request_type_length(tmp_header.get_accept_type(), _file.file_size());
    //    tmp_header.add_serverheader_request_end();

    //    write(client_socket, tmp_header.get_server_header().c_str(), tmp_header.get_server_header().size());

    //    write(client_socket, _file.get_file_str(), 1024);
    //    for (; !_file.file_EOF();)
    //        write(client_socket, _file.get_file_str(), 1024);
    //}

    argv_this.DEL_epoll_evs(client_socket);
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
        file_io _file(_config._web_file_path + tmp_header.get_request_path(), std::ios::in, 1024);
        tmp_header.add_server_header_request_status();
        tmp_header.add_server_header_request_type_length(tmp_header.get_accept_type(), _file.file_size());
        tmp_header.add_serverheader_request_end();

        write(argv_client_socket, tmp_header.get_server_header().c_str(), tmp_header.get_server_header().size());

        write(argv_client_socket, _file.get_file_str(), 1024);
        for (; !_file.file_EOF();)
            write(argv_client_socket, _file.get_file_str(), 1024);
    }
    return;
}



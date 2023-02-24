#pragma once
#include "epoll_server.h"
#include "http_header.h"
#include "file_io.h"
#include "unistd.h"


class http_server
{
private:
    struct config
    {
        std::string _IP;
        int _PORT;

        std::string _web_file_path;
        std::string _web_default;
    };
public:
    http_server();
    ~http_server();

public:
    void start();
    void set_config(std::string argv_ip, int argv_port, std::string argv_web_file_path, std::string argv_web_default = "/index.html");

private:
    void client_read_write(epoll_event argv_ep_ev, epoll_server& argv_this);
    void web_file_read(int& argv_client_socket, std::string& argv_client_header);

private:
    config _config;
    epoll_server* _epoll_server;
};

//inline void l_client_read_write(epoll_event argv_ep_ev, epoll_server& argv_this)
//{
//    int client_socket = argv_ep_ev.data.fd;
//
//    char* buff = new char[1024];
//    std::string buff_plus;
//
//    for (int read_size = 1024; read_size == 1024; )
//    {
//        memset(buff, 0, 1024);
//        read_size = read(client_socket, buff, 1024);
//        //cout << read_size << endl;
//        buff_plus += buff;
//    }
//    delete[](buff);
//
//    //cout << buff_plus << endl;
//    //web_file_read(client_socket, buff_plus);
//    std::string& argv_client_header = buff_plus;
//    http_header tmp_header(argv_client_header);
//    if (tmp_header.client_header_is_ok())
//    {
//        std::cout << "请求路径" << tmp_header.get_request_path() << std::endl;
//        file_io _file("." + tmp_header.get_request_path(), std::ios::in, 1024);
//        tmp_header.add_server_header_request_status();
//        tmp_header.add_server_header_request_type_length(tmp_header.get_accept_type(), _file.file_size());
//        tmp_header.add_serverheader_request_end();
//
//        write(client_socket, tmp_header.get_server_header().c_str(), tmp_header.get_server_header().size());
//
//        write(client_socket, _file.get_file_str(), 1024);
//        for (; !_file.file_EOF();)
//            write(client_socket, _file.get_file_str(), 1024);
//    }
//
//    argv_this.DEL_epoll_evs(client_socket);
//    close(client_socket);
//}




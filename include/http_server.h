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
    //void client_read_write(epoll_event argv_ep_ev, epoll_server& argv_this);
    //void client_read_write(epoll_event argv_ep_ev, epoll_server& argv_this);
    void client_read_write(int argv_client_socket_fd);
    void web_file_read(int& argv_client_socket, std::string& argv_client_header);

private:
    config _config;
    epoll_server* _epoll_server;
};






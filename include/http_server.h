#pragma once
#include "epoll_server.h"
#include "http_header.h"
#include "file_io.h"
#include "log.h"
#include "unistd.h"
#include <string>


class http_server
{
private:
    struct config
    {
        std::string _IP;
        int _PORT;

        std::string _web_fpath;
        std::string _web_default_index_fname;

        std::string _log_path;
        std::string _log_fname;
    };
public:
    http_server();
    ~http_server();

public:
    void start();
    void set_config(std::string argv_ip, int argv_port, std::string argv_web_fpath = ".", std::string argv_web_default_index_fname = "/index.html", std::string argv_log_path = "./log", std::string argv_log_fname = "log.txt");

private:
    //void client_read_write(int argv_client_fd, epoll_server& argv_this);
    void client_read_write(int argv_client_fd);
    void web_file_read(int& argv_client_socket, std::string& argv_client_header);
    const std::ios_base::openmode file_read_b(const std::string& argv_file_type);

private:
    config _config;
    epoll_server* _epoll_server;
};






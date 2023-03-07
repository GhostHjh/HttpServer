#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <functional>
#include <atomic>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/fcntl.h>
#include <ThreadPool.hpp>

#include "fcntl.h"

class epoll_server
{
private:
    epoll_server();
    ~epoll_server();

public:
    static epoll_server* GET();
    void config(std::string argv_ip = "0.0.0.0", int argv_port = 22222, int argv_listen_size = 1024);
    void status();
    void stop();

public:
    //const bool set_client_func(std::function<void(epoll_event argv_event_ev, epoll_server& argv_this)> argv_client_func);
    //const bool set_client_func(std::function<void(int, epoll_server&)> argv_client_func);
    const bool set_client_func(std::function<void(int)> argv_client_func);
    const bool set_client_func();

    void ADD_epoll_evs(int& argv_client_socket);
    void DEL_epoll_evs(int& argv_client_socket);

private:
    const bool socket_init();
    const bool epoll_init();
    void fcntl_fd(int& argv_fd);

private:
    //std::atomic<bool> _status;
    bool _status;
    std::string _IP;
    int _PORT;

    int _socket_fd;
    int _epoll_fd;
    int _listen_size;
    //epoll_event* _epoll_evs;

    static epoll_server* _epoll_server;
private:
    //std::function<void(epoll_event argv_event_ev, epoll_server& argv_this)> client_func;
    //std::function<void(int client_fd, epoll_server& argv_this)> client_func;
    std::function<void(int client_fd)> client_func;
};









#include "epoll_server.h"
#include "ThreadPool.hpp"
#include "http_header.h"
#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h>


int epoll_server::_epoll_fd = 0;

epoll_server::epoll_server(std::string argv_ip, int argv_port, int argv_listen_size)
    : _IP(argv_ip), _PORT(argv_port), _socket_fd(0), _listen_size(argv_listen_size)
{
    socket_init();
    epoll_init();
    _epoll_evs = new epoll_event[_listen_size];
    _status = true;
}

epoll_server::~epoll_server()
{
    close(_epoll_fd);
    close(_socket_fd);
    delete[](_epoll_evs);
}

const bool epoll_server::socket_init()
{
    if (_socket_fd < 1)
    {
        _socket_fd = socket(AF_INET, SOCK_STREAM, 0) ;
    
        sockaddr_in socket_addr;
        socket_addr.sin_family = AF_INET;
        socket_addr.sin_addr.s_addr = inet_addr(_IP.c_str());
        socket_addr.sin_port = htons(static_cast<uint16_t>(_PORT));

        if (bind(_socket_fd, reinterpret_cast<sockaddr*>(&socket_addr), static_cast<socklen_t>(sizeof(socket_addr))) == -1)
            return false;

        if (listen(_socket_fd, _listen_size) == -1)
            return false;
    }
    
    return true;
}

const bool epoll_server::epoll_init()
{
    if (!socket_init())
        return false;

    if (_epoll_fd < 1)
    {
        _epoll_fd = epoll_create(_listen_size);

        epoll_event epoll_ev;
        epoll_ev.data.fd = _socket_fd;
        //epoll_ev.events = EPOLLIN | EPOLLET;
        epoll_ev.events = EPOLLIN | EPOLLET;

        epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _socket_fd, &epoll_ev);

        if (_epoll_fd < 1)
            return false;
    }

    return true;

}

void epoll_server::file_fcntl(int& argv_fd)
{
    int file_flag = fcntl(argv_fd, F_GETFL);
    file_flag |= O_NONBLOCK;
    fcntl(argv_fd, F_SETFL, file_flag);
}

void epoll_server::start()
{
    std::cout << set_client_func() <<  std::endl;

    if (!socket_init() || !epoll_init() || !set_client_func())
    {
        //std::logic_error("启动失败");
        return;
    }


    int epoll_wait_return = 0;
    int for_int = 0;

    ThreadPool _threadpool;
    //signal(SIGPIPE,SIG_IGN);
    for(; _status;)
    {
        epoll_wait_return = epoll_wait(_epoll_fd, _epoll_evs, _listen_size, -1);
        std::cout << "有事件" << epoll_wait_return << "个\n";

        for (for_int = 0; for_int < epoll_wait_return; ++for_int)
        {
            if (_epoll_evs[for_int].data.fd == _socket_fd)
            {
                int client_socket = accept(_socket_fd, nullptr, nullptr);
                ADD_epoll_evs(client_socket);
            }
            else 
            {
                int tmp_client_fd = _epoll_evs[for_int].data.fd;
                _threadpool.add_task(client_func, tmp_client_fd);
                //client_func(_epoll_evs[for_int].data.fd);
            }
        }
    }
}

void epoll_server::stop()
{
    //_status.store(false);
    _status = false;
}

//const bool epoll_server::set_client_func(std::function<void (epoll_event argv_event_ev, epoll_server &)> argv_client_func)
//{
//    client_func = std::move(argv_client_func);
//    //client_func = std::move(argv_client_func);
//    return static_cast<bool>(client_func);
//}
const bool epoll_server::set_client_func(std::function<void (int argv_client_socket_fd)> argv_client_func)
{
    client_func = std::move(argv_client_func);
    //client_func = std::move(argv_client_func);
    return static_cast<bool>(client_func);
}
const bool epoll_server::set_client_func()
{ 
    return static_cast<bool>(client_func);
}

void epoll_server::ADD_epoll_evs(int argv_client_socket)
{
    epoll_event tmp_ep_ev;
    tmp_ep_ev.events = EPOLLIN | EPOLLET;
    file_fcntl(argv_client_socket);
    tmp_ep_ev.data.fd = argv_client_socket;

    epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, argv_client_socket, &tmp_ep_ev);
}

void epoll_server::DEL_epoll_evs(int argv_client_socket)
{
    epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, argv_client_socket, nullptr);  
}




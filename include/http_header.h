#pragma  once
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <initializer_list>

class http_header
{
public:
    http_header() = delete;
    http_header(std::string argv_client_header_str);
    ~http_header();

//client_header有关的函数
public:
    void show();
    bool client_header_is_ok();

    const std::string& operator[](std::string argv_key);
    const std::string get_request_type();
    const std::string get_request_path();
    const std::string get_request_http_version();
    const std::string get_accept_type();
    const std::map< std::string, std::string > get_post();

private:
    void client_header_map_init();

private:
    //存储客户端发来的header字符串和解析完的键致对
    std::string client_header_str;
    std::map< std::string, std::string > client_header_map;
    std::map< std::string, std::string > client_header_post_map;


//server_header有关的函数
public:
    void add_server_header_request_status(double argv_request_http_version = 1.1, int argv_request_status = 200, std::string argv_request_status_why = "OK");
    void add_server_header_request_type_length(std::string argv_request_type, int argv_request_length);
    //void add_server_header_request_length(int argv_request_length);   
    void add_server_header(std::initializer_list< std::string > argv_pair_s);
    void add_serverheader_request_end();

    std::string& add_server_header(std::string argv_key);
    
    const std::string get_server_header();

private:

private:
    //存储用于回应客户端的header字符串和键值对
    std::string server_header_str;
    std::map< std::string, std::string > server_header_map;
};








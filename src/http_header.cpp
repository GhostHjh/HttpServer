#include <cstdlib>
#include <http_header.h>
#include <iostream>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>

http_header::http_header(std::string argv_client_header_str, std::string argv_path_defult_str)
    : client_header_str(argv_client_header_str), path_default(argv_path_defult_str)
{
    client_header_map_init();
}

http_header::~http_header() {}

void http_header::show()
{
    std::cout << client_header_map["request_type"] << ' ' << client_header_map["request_path"] << ' ' << client_header_map["request_http_version"] << "\r\n";

    for (auto i : client_header_map)
    {
        if (i.first != "request_type" && i.first != "request_path" && i.first != "request_http_version")
            std::cout << i.first << " : " << i.second << std::endl;
    }

    std::cout << std::endl << std::endl;

    std::cout << "POST_str:\t";
    for (auto i = client_header_post_map.begin(); i != client_header_post_map.end(); ++i)
        std::cout << i->first << '=' << i->second << '&';
}

bool http_header::client_header_is_ok()
{
    return client_header_map.count("request_type") && client_header_map.count("request_path") && client_header_map.count("request_http_version");
}

const std::string& http_header::operator[](std::string argv_key)
{
    return client_header_map[argv_key];
}
const std::string http_header::get_request_type()
{
    return client_header_map["request_type"];
}

const std::string http_header::get_request_path()
{
    return client_header_map["request_path"];
}

const std::string http_header::get_request_http_version()
{
    return client_header_map["request_http_version"];
}

const std::string http_header::get_accept_type()
{
    std::string return_str = client_header_map["Accept"];
    int return_str_index = return_str.find(',');
    if (return_str_index == 0)
        return return_str;
    else
        return return_str.substr(0, return_str_index);
}

const std::map< std::string, std::string > http_header::get_post()
{
    return client_header_post_map;
}

void http_header::client_header_map_init()
{
    int client_header_str_index = 0;

    //获取客户端header的请求类型
    if (client_header_str[0] == 'G' && client_header_str[1] == 'E' && client_header_str[2] == 'T')
        client_header_str_index  = 4, client_header_map["request_type"] = "GET";
    else if (client_header_str[0] == 'P' && client_header_str[1] == 'O' && client_header_str[2] == 'S' && client_header_str[3] == 'T')
        client_header_str_index = 5, client_header_map["request_type"] = "POST";

    //获取客户端请求的路径
    for (; client_header_str[client_header_str_index] != ' '; ++client_header_str_index)
        client_header_map["request_path"] += client_header_str[client_header_str_index];
    path_default_init();
    ++client_header_str_index;

    //获取客户端发送请求的HTTP版本
    for (; client_header_str[client_header_str_index] != '\r' && client_header_str[client_header_str_index] != '\n'; ++client_header_str_index)
        client_header_map["request_http_version"] += client_header_str[client_header_str_index];
    client_header_str_index +=2;

    //获取其他
    std::string header_key;
    std::string header_value;
    for (int &i = client_header_str_index; i < client_header_str.size() && client_header_str[i] != '\r' && client_header_str[i+1] != '\n'; )
    {
        for (; i < client_header_str.size() && client_header_str[i] != ':'; ++i)
            header_key += client_header_str[i];
        
        i += 2;

        for (; i < client_header_str.size() && client_header_str[i] != '\r' && client_header_str[i] != '\n'; ++i)
            header_value += client_header_str[i];

        i += 2;

        client_header_map[header_key] = header_value;
        header_key.clear();
        header_value.clear();
    }

    //如果是POST请求方式, 就获取POST报文
    if (client_header_map["request_type"] == "POST" && client_header_str_index < client_header_str.size())
    {
        int &i = client_header_str_index;
        i += 2;

        for (; i < client_header_str.size() && i < client_header_str.size(); )
        {
            for (; i < client_header_str.size() && client_header_str[i] != '='; ++i)
                header_key += client_header_str[i];

            ++i;

            for (; i < client_header_str.size() && client_header_str[i] != '&'; ++i)
                header_value += client_header_str[i];

            ++i;

            client_header_post_map[header_key] = header_value;
            header_key.clear();
            header_value.clear();
        }
    }
}

void http_header::path_default_init()
{
    if (client_header_map["request_path"] == "/")
        client_header_map["request_path"] = path_default;
}

//*************************************************************************************************

void http_header::add_server_header_request_status(double argv_request_http_version, int argv_request_status, std::string argv_request_status_why)
{
    server_header_str += "HTTP/" + std::to_string(argv_request_http_version).substr(0, 3) + ' ' + std::to_string(argv_request_status) + ' ' + argv_request_status_why + "\r\n";
}

void http_header::add_server_header_request_type_length(std::string argv_request_type, int argv_request_length)
{
    server_header_str += "Content-Type: " + argv_request_type +"\r\nContent-length: " + std::to_string(argv_request_length) + "\r\n";
}

//void http_header::add_server_header_request_length(int argv_request_length)
//{
//    server_header_str += "Content-Type: " + get_accept_type() +"\r\nContent-length: " + std::to_string(argv_request_length) + "\r\n";
//}

void http_header::add_server_header(std::initializer_list< std::string > argv_pair_s)
{
    if (argv_pair_s.size() % 2 != 0)
        std::logic_error("参数不够");
    
    for (auto i = argv_pair_s.begin(); i != argv_pair_s.end(); i += 2)
    {
        server_header_map[*i] = *(i+1);
    }
    
    for(auto i : server_header_map)
        server_header_str += i.first + " : " + i.second + "\r\n";

}

void http_header::add_serverheader_request_end()
{
    server_header_str += "\r\n";
}

const std::string http_header::get_server_header()
{    
    return server_header_str;
}

std::string& http_header::add_server_header(std::string argv_key)
{
    return server_header_map[argv_key];
}




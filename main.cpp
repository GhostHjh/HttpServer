#include <iostream>
#include "http_server.h"

int main(int argc, const char** argv)
{
//    std::cout << "Hello World!\n";
    http_server tmp_server;
    tmp_server.set_config("0.0.0.0", 22226, "./web", "/index.html");
    tmp_server.start();


    return 0;
}



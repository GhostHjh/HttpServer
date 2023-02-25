#include <iostream>
#include "http_server.h"

int main(int argc, const char** argv)
{
//    std::cout << "Hello World!\n";
    try {
        http_server tmp_server;
        tmp_server.set_config("0.0.0.0", 22229, "./web", "/index.html");
        tmp_server.start();   
    } catch(const char* argv_catch) {
        std::cout << "有错误" << argv_catch << std::endl;
    }
    



    return 0;
}



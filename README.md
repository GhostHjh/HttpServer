# HttpServer

Http静态文件服务器

## 安装和使用

> 将 **头文件(http_header.h)** 和 **cpp文件(http_header.cpp)** 文件添加到工程
>
> 位于根目录的 **main.cpp** 和 **CMakeLista.txt** 为示范,

## 运行服务器

> 直接创建对象并设置参数
>
> ```cpp
> //创建对象
> http_server m_http_server;
>
> //设置参数
> //第一,二个参数为要绑定的ip，端口
> //第三个参数为web文件所在的文件夹(默认为"./")
> //第四个参数为web文件夹中默认打开的文件(默认为"index.html")
> m_http_server.set_config(std::string IP, int PORT, std::string web_file_path, std::string web_file_default);
>
> //启动
> m_http_server.start();
> ```
>



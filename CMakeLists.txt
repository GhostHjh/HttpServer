cmake_minimum_required(VERSION 3.0)

project(file_io)

set(main_file_name main.cpp)

include_directories(include)

aux_source_directory(src include_src)

add_executable(main ${main_file_name} ${include_src})

add_compile_options(-std=c++11)

target_link_libraries(main -pthread)

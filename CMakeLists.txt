cmake_minimum_required(VERSION 3.0)

project(simple_getServerTime)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads)

add_executable(client client.cpp)
add_executable(server server.cpp)

target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(client boost_system)
target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})

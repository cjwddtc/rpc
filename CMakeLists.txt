#cmake_minimum_required(VERSION 3.28)
project(main)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED COMPONENTS system serialization)
add_library(rpclib  server.cpp client.cpp)

add_executable(main main.cpp
        api.h)

target_include_directories(rpclib PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(rpclib ${Boost_LIBRARIES})

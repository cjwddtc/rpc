#cmake_minimum_required(VERSION 3.28)
project(main)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED COMPONENTS system serialization)
add_library(rpclib  server_network.cpp client_network.cpp)

add_executable(server server.cpp)
add_executable(client client.cpp)

target_include_directories(rpclib PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(rpclib ${Boost_LIBRARIES})
target_link_libraries(server rpclib)
target_link_libraries(client rpclib)
add_executable(main main.cpp)
target_include_directories(main PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(main ${Boost_LIBRARIES})
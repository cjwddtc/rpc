cmake_minimum_required(VERSION 3.28)
project(main)

set(CMAKE_CXX_STANDARD 20)
find_package(Boost REQUIRED COMPONENTS system serialization)

add_executable(main main.cpp
        api.h)

target_link_libraries(main)
target_link_libraries()
target_include_directories(main PRIVATE ${Boost_INCLUDE_DIRS})

target_link_libraries(main ${Boost_LIBRARIES})

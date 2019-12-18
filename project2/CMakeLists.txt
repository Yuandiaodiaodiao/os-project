cmake_minimum_required(VERSION 3.10)
project(project2)

set(CMAKE_CXX_STANDARD 11)

add_executable(project2 main.cpp)
TARGET_LINK_LIBRARIES( project2
        pthread
        )
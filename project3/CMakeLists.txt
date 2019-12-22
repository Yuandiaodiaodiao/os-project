cmake_minimum_required(VERSION 3.10)
project(project3)

set(CMAKE_CXX_STANDARD 11)

add_executable(project3 main.cpp)
TARGET_LINK_LIBRARIES( project3
        pthread
        )
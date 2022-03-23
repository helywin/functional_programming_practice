cmake_minimum_required(VERSION 3.21)
project(functional_programming_practice)

set(CMAKE_CXX_COMPILER clang++-12)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++2a")

add_subdirectory(src)

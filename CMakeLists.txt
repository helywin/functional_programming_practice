cmake_minimum_required(VERSION 3.20)
project(functional_programming_practice)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /utf-8")
else ()
    set(CMAKE_CXX_COMPILER clang++-14)
#    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
#    set(TBB_Dir /usr/local/lib/TBB/cmake)
    find_package(TBB REQUIRED)
endif ()


add_subdirectory(src)

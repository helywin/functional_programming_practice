cmake_minimum_required(VERSION 3.21)
project(functional_programming_practice)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /utf-8")
else ()
    set(CMAKE_CXX_STANDARD 17)
endif ()

add_subdirectory(src)

cmake_minimum_required(VERSION 3.20)
project(functional_programming_practice)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(range-v3 REQUIRED)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /utf-8")
else ()
#    set(CMAKE_CXX_COMPILER /usr/local/gcc-11.2/bin/g++-11.2)
    set(CMAKE_CXX_STANDARD 20)
    find_package(TBB REQUIRED)
endif ()
find_package(range-v3 CONFIG REQUIRED)

add_subdirectory(src)

cmake_minimum_required(VERSION 3.21)
project(functional_programming_practice)

if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /utf-8")
else ()
    set(CMAKE_CXX_COMPILER g++-11)
    set(CMAKE_CXX_STANDARD 17)
#    set(TBB_Dir /usr/local/lib/TBB/cmake)
    find_package(TBB REQUIRED)
endif ()


add_subdirectory(src)

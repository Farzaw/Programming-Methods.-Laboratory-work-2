cmake_minimum_required(VERSION 3.14)
project(flight_search LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(flight_search main.cpp)

if(MSVC)
    target_compile_options(flight_search PRIVATE /W4)
else()
    target_compile_options(flight_search PRIVATE -Wall -Wextra -O2)
endif()

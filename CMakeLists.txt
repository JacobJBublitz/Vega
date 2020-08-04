cmake_minimum_required(VERSION 3.13)

project(Vega)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Vega_Server)
add_subdirectory(src)

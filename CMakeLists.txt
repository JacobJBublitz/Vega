cmake_minimum_required(VERSION 3.13)

project(Vega)

add_subdirectory(third_party)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Vega_Server src/vega/util/string.h)
add_subdirectory(src)

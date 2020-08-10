cmake_minimum_required(VERSION 3.13)

project(Vega)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

add_subdirectory(third_party/libuv)
set(BUILD_UVW_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/uvw)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Vega_Server)
add_subdirectory(src)

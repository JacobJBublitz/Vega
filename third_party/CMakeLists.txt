set(BUILD_TESTING OFF CACHE INTERNAL "")

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(json)

add_subdirectory(libuv)

set(BUILD_UVW_LIBS OFF CACHE INTERNAL "")
set(USE_LIBCPP OFF CACHE INTERNAL "")
add_subdirectory(uvw)

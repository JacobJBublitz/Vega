set(COMMON_LIBRARIES
    uv_a # Staticly linked version of libuv
    uvw::uvw
    nlohmann_json::nlohmann_json
)
set(COMMON_SOURCES
    vega/compiler.h
    vega/util/varint.h
)

target_compile_options(Vega_Server PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W /WX>
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:-Wall -Wextra -pedantic -Werror>
)
target_include_directories(Vega_Server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(Vega_Server PRIVATE
    ${COMMON_LIBRARIES}
)
target_sources(Vega_Server PRIVATE
    ${COMMON_SOURCES}
    vega/server/client.cc
    vega/server/client.h
    vega/server/main.cc
    vega/server/protocol/packet.cc
    vega/server/protocol/packet.h
    vega/server/protocol/protocol.h
    vega/server/protocol/protocol_1.16.cc
    vega/server/protocol/protocol_1.16.h
)

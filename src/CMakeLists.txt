set(COMMON_LIBRARIES
    uv_a # Staticly linked version of libuv
    uvw::uvw
)
set(COMMON_SOURCES
    vega/compiler.h
    vega/net/ip.h
    vega/net/tcp.cc
    vega/net/tcp.h
    vega/net/detail/tcp.h
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

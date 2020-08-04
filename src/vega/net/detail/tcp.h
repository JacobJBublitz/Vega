#pragma once

#include "vega/compiler.h"

#if defined(VEGA_OS_LINUX) || defined(VEGA_OS_MACOS)

namespace vega::net::detail {

using TcpClientHandle = int;
using TcpServerHandle = int;

}  // namespace vega::net::detail

#elif defined(VEGA_OS_WINDOWS)

extern "C" {
#include <WS2tcpip.h>
#include <WinSock2.h>
}

namespace vega::net::detail {

using TcpClientHandle = ::SOCKET;
using TcpServerHandle = ::SOCKET;

}  // namespace vega::net::detail

#endif
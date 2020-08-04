#include "vega/net/tcp.h"

#include "vega/compiler.h"

#ifdef VEGA_OS_WINDOWS

extern "C" {
#include <WinSock2.h>
}

#pragma comment(lib, "Ws2_32.lib")

namespace vega::net {

TcpClient::TcpClient(TcpClient &&other) {
  handle_ = other.handle_;
  other.handle_ = INVALID_SOCKET;
}

TcpClient::~TcpClient() {
  if (handle_ == INVALID_SOCKET) return;

  ::shutdown(handle_, SD_BOTH);
  ::closesocket(handle_);
}

size_t TcpClient::Read(std::span<std::byte> buffer) {
  size_t total_read = 0;

  while (total_read != buffer.size()) {
    int res =
        ::recv(handle_, reinterpret_cast<char *>(buffer.data() + total_read),
               static_cast<int>(buffer.size() - total_read), 0);
    if (res == 0) {
      // Socket was closed
      // TODO: Handle gracefully
      throw std::runtime_error("Socket was closed");
    } else if (res == SOCKET_ERROR) {
      auto error = ::WSAGetLastError();
      // Error occurred
      // TODO: Handle EWOULDBLOCK?
      // TODO: Better error message
      throw std::runtime_error("An error occurred while reading from socket");
    }

    total_read += res;
  }

  return total_read;
}

size_t TcpClient::Write(std::span<const std::byte> bytes) {
  size_t total_written = 0;

  while (total_written != bytes.size()) {
    int res = ::send(
        handle_, reinterpret_cast<const char *>(bytes.data() + total_written),
        static_cast<int>(bytes.size() - total_written), 0);
    if (res == SOCKET_ERROR) {
      auto error = ::WSAGetLastError();

      throw std::runtime_error("An error occurred while writing to socket");
    }

    total_written += res;
  }

  return total_written;
}

TcpServer::TcpServer() {
  handle_ = ::socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
  if (handle_ == INVALID_SOCKET) {
    // TODO: Handle error
    throw std::runtime_error("Failed to create socket");
  }
}

TcpServer::~TcpServer() {
  ::shutdown(handle_, SD_BOTH);
  ::closesocket(handle_);
}

void TcpServer::Bind(uint16_t port) {
  ::sockaddr_in6 saddr{};
  saddr.sin6_family = AF_INET6;
  saddr.sin6_port = htons(port);
  saddr.sin6_addr = in6addr_any;

  if (::bind(handle_, reinterpret_cast<const ::sockaddr *>(&saddr),
             sizeof(saddr)) == SOCKET_ERROR) {
    throw std::runtime_error("Unable to bind to port");
  }
}

void TcpServer::Bind(Ipv4Address addr, uint16_t port) {
  ::sockaddr_in saddr{};
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  std::memcpy(&saddr.sin_addr, &addr, sizeof(Ipv4Address));

  if (::bind(handle_, reinterpret_cast<const ::sockaddr *>(&saddr),
             sizeof(saddr)) == SOCKET_ERROR) {
    // TODO: Handle error
    throw std::runtime_error("Failed to bind socket to address");
  }
}

void TcpServer::Listen(uint32_t backlog) {
  if (::listen(handle_, backlog)) {
    auto err = ::WSAGetLastError();
    // TODO: Handle error
    throw std::runtime_error("Failed to listen for connections");
  }
}

TcpClient TcpServer::Accept() {
  SOCKET client = ::accept(handle_, nullptr, nullptr);
  if (client == INVALID_SOCKET) {
    auto err = ::WSAGetLastError();
    // TODO: Handle error
    throw std::runtime_error("Failed to accept incoming connection");
  }

  return TcpClient(client);
}

}  // namespace vega::net

#else

#endif

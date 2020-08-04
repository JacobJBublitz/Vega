#pragma once

#include <cstdint>
#include <span>

#include "vega/net/detail/tcp.h"
#include "vega/net/ip.h"

namespace vega::net {

class TcpServer;

class TcpClient {
  friend class TcpServer;

 public:
  TcpClient(TcpClient&& other);

  TcpClient(const TcpClient&) = delete;
  void operator=(const TcpClient&) = delete;

  ~TcpClient();

  size_t Read(std::span<std::byte> bytes);

  size_t Peek(std::span<std::byte> bytes);

  size_t Write(std::span<const std::byte> buffer);

 private:
  detail::TcpClientHandle handle_;

  inline TcpClient(detail::TcpClientHandle handle) : handle_(handle) {}
};

class TcpServer {
 public:
  constexpr static uint32_t kDefaultBacklog = 10;

  TcpServer();

  TcpServer(const TcpServer&) = delete;
  void operator=(const TcpServer&) = delete;

  ~TcpServer();

  void Bind(uint16_t port);
  void Bind(Ipv4Address addr, uint16_t port);
  void Bind(Ipv6Address addr, uint16_t port);

  void Listen(uint32_t backlog = kDefaultBacklog);

  TcpClient Accept();

 private:
  detail::TcpServerHandle handle_;
};

}  // namespace vega::net

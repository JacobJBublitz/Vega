#pragma once

#include <cstdint>
#include <vector>

#include "vega/util/stream.h"

namespace vega {
namespace server {

class TcpSocket final : public util::Stream {
 public:
  using HandleType = void *;

  explicit TcpSocket(HandleType handle);

  ~TcpSocket() final;

  size_t Read(void *buffer, size_t buffer_size) final;

  size_t Write(const void *buffer, size_t buffer_size) final;

 private:
  HandleType handle_;
};

class TcpServerSocket {
 public:
  using HandleType = void *;

  explicit TcpServerSocket(uint16_t port);

  ~TcpServerSocket();

  TcpSocket Accept();

 private:
  HandleType handle_;
};

}  // namespace server
}  // namespace vega

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "uvw.hpp"
#include "vega/server/protocol/protocol_1.16.h"

namespace vega::server {

class ClientConnection {
 public:
  ClientConnection(uvw::TCPHandle& socket);
  ~ClientConnection();

 private:
  uvw::TCPHandle& socket_;
  std::unique_ptr<protocol::Protocol_1_16> protocol_;

  void OnData(std::span<const std::byte> data);

  static void OnDataHandler(uvw::DataEvent& event, uvw::TCPHandle& handle);
};

}  // namespace vega::server

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "vega/net/tcp.h"
#include "vega/server/protocol/protocol_1.16.h"

namespace vega::server {

class ClientConnection {
 public:
  ClientConnection(net::TcpClient client);
  ~ClientConnection();

  void Poll();

 private:
  net::TcpClient socket_;
  std::unique_ptr<protocol::Protocol_1_16> protocol_;
};

}  // namespace vega::server

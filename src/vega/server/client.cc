#include "vega/server/client.h"

#include <iostream>

#include "vega/server/protocol/protocol_1.16.h"

namespace vega::server {

namespace {

int32_t ReadVarInt(net::TcpClient &socket) {
  int32_t value = 0;
  for (size_t i = 0; i < protocol::Packet::kMaxVarIntBytes; ++i) {
    std::byte b;
    socket.Read({&b, 1});
    value = (value << 7) | (std::to_integer<int32_t>(b) & 0x7f);

    if ((b & std::byte{0x80}) != std::byte{0}) break;
  }

  return value;
}

}  // namespace

ClientConnection::ClientConnection(net::TcpClient socket)
    : socket_(std::move(socket)),
      protocol_(std::make_unique<protocol::Protocol_1_16>(socket)) {}

ClientConnection::~ClientConnection() = default;

void ClientConnection::Poll() {
  std::cout << "Read pkt len\n";
  int32_t len = ReadVarInt(socket_);
  std::cout << "pkt len: " << len << "\n";

  std::vector<std::byte> buffer{static_cast<size_t>(len)};
  socket_.Read(buffer);

  protocol::Packet pkt(buffer);

  if (!protocol_->HandlePacket(pkt)) {
    throw std::runtime_error("Failed to handle packet");
  }
}

}  // namespace vega::server
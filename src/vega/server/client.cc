#include "vega/server/client.h"

#include <iostream>

#include "vega/server/protocol/protocol_1.16.h"

namespace vega::server {

namespace {

int32_t ReadVarInt(std::span<const std::byte> data, size_t *num_bytes) {
  int32_t result = 0;
  size_t i;
  for (i = 0; i < protocol::Packet::kMaxVarIntBytes; ++i) {
    uint32_t value = std::to_integer<uint32_t>(data[i]);
    result |= (value & 0x7f) << (7 * i);

    if ((value & 0x80) == 0) break;
  }
  *num_bytes = i + 1;

  return result;
}

}  // namespace

ClientConnection::ClientConnection(uvw::TCPHandle &socket)
    : socket_(socket),
      protocol_(std::make_unique<protocol::Protocol_1_16>(socket)) {
  socket_.on<uvw::DataEvent>(OnDataHandler);
}

ClientConnection::~ClientConnection() = default;

void ClientConnection::OnData(std::span<const std::byte> data) {
  // TODO: Use data as part of a stream. Excess data should be copied to a
  // buffer until a full packet is created.

  size_t varint_bytes;
  int32_t len = ReadVarInt(data, &varint_bytes);

  protocol::Packet pkt(data.subspan(varint_bytes, len));

  if (!protocol_->HandlePacket(pkt)) {
    // If we are unable to handle the packet, close the connection to the
    // client.
    socket_.close();
    return;
  }
}

void ClientConnection::OnDataHandler(uvw::DataEvent &event,
                                     uvw::TCPHandle &handle) {
  auto conn = handle.data<ClientConnection>();
  conn->OnData(
      {reinterpret_cast<const std::byte *>(event.data.get()), event.length});
}

}  // namespace vega::server

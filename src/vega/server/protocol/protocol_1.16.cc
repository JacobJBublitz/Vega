#include "protocol_1.16.h"

#include <iostream>

namespace vega::server::protocol {

namespace {

constexpr int32_t kPktHandshakeId = 0x00;

constexpr int32_t kPktStatusRequestId = 0x00;
constexpr int32_t kPktStatusResponseId = 0x00;
constexpr int32_t kPktStatusPingId = 0x01;
constexpr int32_t kPktStatusPongId = 0x01;

constexpr int32_t kPktLoginStartId = 0x00;
constexpr int32_t kPktLoginDisconnectId = 0x00;

constexpr std::u8string_view kTestStatusResponse =
    u8"{\"version\":{\"name\":\"1.16.1\",\"protocol\":736},\"players\":{"
    u8"\"max\":420,\"online\":69,\"sample\":[]},\"description\":{\"text\":"
    u8"\"Hello, world!\nTesting\nAnother Line\"}}";

}  // namespace

Protocol_1_16::Protocol_1_16(uvw::TCPHandle& socket) : socket_(socket) {}

Protocol_1_16::~Protocol_1_16() = default;

bool Protocol_1_16::HandlePacket(Packet packet) {
  auto packet_id = packet.ReadVarInt();
  std::cout << "Got packet with id " << std::hex << packet_id << std::dec
            << "\n";

  if (state_ == State::kHandshake) {
    if (packet_id != kPktHandshakeId) return false;

    // Handshake packet

    int32_t protocol_version = packet.ReadVarInt();
    std::string server_address = packet.ReadVarString();
    uint16_t server_port = packet.ReadUnsignedShort();
    State next_state = State{packet.ReadVarInt()};

    std::cout << "  Protocol version: " << protocol_version << "\n"
              << "  Server address: " << server_address << "\n"
              << "  Server port: " << server_port << "\n"
              << "  Next state: " << static_cast<uint32_t>(next_state) << "\n";

    state_ = next_state;
    return true;
  } else if (state_ == State::kStatus) {
    switch (packet_id) {
      case kPktStatusRequestId:
        return HandlePacketStatusRequest(packet);
      case kPktStatusPingId:
        return HandlePacketStatusPing(packet);
      default:
        std::cerr << "Unknown status packet " << std::hex << packet_id
                  << std::dec << "\n";
        return false;
    }
  } else if (state_ == State::kLogin) {
    std::cerr << "Unknown login packet " << std::hex << packet_id << std::dec
              << "\n";
    return false;
  } else if (state_ == State::kPlay) {
    std::cerr << "Unknown play packet " << std::hex << packet_id << std::dec
              << "\n";
    return false;
  } else {
    std::cerr << "Client is in an unknown state "
              << static_cast<uint32_t>(state_) << "\n";
    return false;
  }
}

bool Protocol_1_16::HandlePacketStatusPing(Packet packet) {
  int64_t payload = packet.ReadLong();

  SendPacketStatusPong(payload);
  return true;
}

bool Protocol_1_16::HandlePacketStatusRequest(Packet packet) {
  std::cout << "Handling status request\n";
  SendPacketStatusResponse(kTestStatusResponse);

  return true;
}

void Protocol_1_16::SendPacket(Packet packet) {
  size_t buffer_size = util::kMaxVarIntBytes + packet.Data().size();
  auto buffer = new char[buffer_size];

  auto data = std::unique_ptr<char[]>(buffer);

  auto pkt_len_size_bytes = util::WriteVarInt(
      static_cast<int32_t>(packet.Data().size()),
      std::span{reinterpret_cast<std::byte*>(buffer), buffer_size});
  // TODO: Handle error

  std::memcpy(data.get() + pkt_len_size_bytes, packet.Data().data(),
              packet.Data().size());
  socket_.write(
      std::move(data),
      static_cast<unsigned int>(pkt_len_size_bytes + packet.Data().size()));
}

void Protocol_1_16::SendPacketStatusPong(int64_t payload) {
  Packet packet;
  packet.WriteVarInt(kPktStatusPongId);
  packet.WriteLong(payload);

  SendPacket(packet);
}

void Protocol_1_16::SendPacketStatusResponse(std::u8string_view response) {
  Packet packet;
  packet.WriteVarInt(kPktStatusResponseId);
  packet.WriteVarString(response);

  SendPacket(packet);
}

void Protocol_1_16::SendDisconnect() {
  // TODO: Implement
}

}  // namespace vega::server::protocol

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
    u8"{\"version\":{\"name\":\"1.16\",\"protocol\":735\"},\"players\":{"
    u8"\"max\":"
    u8"1,\"online\":0,\"sample\":[]},\"description\":{\"text\":\"\"}";

}  // namespace

Protocol_1_16::Protocol_1_16(net::TcpClient &socket) : socket_(socket) {}

Protocol_1_16::~Protocol_1_16() = default;

bool Protocol_1_16::HandlePacket(Packet packet) {
  auto packet_id = packet.ReadVarInt();

  if (state_ == State::kHandshake) {
    if (packet_id != kPktHandshakeId) return false;

    // Handshake packet
    int32_t protocol_version = packet.ReadVarInt();
    std::string server_address = packet.ReadVarString();
    uint16_t server_port = packet.ReadUnsignedShort();
    State next_state = State{packet.ReadVarInt()};

    std::cout << "Protocol version: " << protocol_version << "\n"
              << "Server address: " << server_address << "\n"
              << "Server port: " << server_port << "\n"
              << "Next state: " << static_cast<uint32_t>(next_state) << "\n";

    return true;
    // } else if (state_ == State::kStatus) {
    //   switch (packet_id) {
    //     case kPktStatusPingId:
    //       return HandlePacketStatusPing(packet);
    //     case kPktStatusRequestId:
    //       return HandlePacketStatusRequest(packet);
    //     default:
    //       return false;
    //   }
  } else {
    return false;
  }
}

bool Protocol_1_16::HandlePacketStatusPing(Packet packet) {
  int64_t payload = packet.ReadLong();

  SendPacketStatusPong(payload);
  return true;
}

bool Protocol_1_16::HandlePacketStatusRequest(Packet packet) {
  SendPacketStatusResponse(kTestStatusResponse);

  return true;
}

void Protocol_1_16::SendPacket(Packet packet) { socket_.Write(packet.Data()); }

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

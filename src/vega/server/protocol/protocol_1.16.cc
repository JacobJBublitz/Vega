#include "protocol_1.16.h"

#include <iostream>

#include "nlohmann/json.hpp"
#include "vega/util/string.h"  // NOLINT(modernize-deprecated-headers)

namespace vega::server::protocol {

namespace {

constexpr int32_t kPktHandshakeId = 0x00;

constexpr int32_t kPktStatusRequestId = 0x00;
constexpr int32_t kPktStatusResponseId = 0x00;
constexpr int32_t kPktStatusPingId = 0x01;
constexpr int32_t kPktStatusPongId = 0x01;

constexpr int32_t kPktLoginStartId = 0x00;
constexpr int32_t kPktLoginDisconnectId = 0x00;

constexpr std::u8string_view kServerDescription = u8"Test server please ignore";
constexpr std::u8string_view kDisconnectText = u8"Not yet implemented";

}  // namespace

Protocol_1_16::Protocol_1_16(uvw::TCPHandle &socket) : socket_(socket) {}

Protocol_1_16::~Protocol_1_16() = default;

bool Protocol_1_16::HandlePacket(Packet &packet) {
  auto packet_id = packet.ReadVarInt();
  std::cout << "Got packet with id " << std::hex << packet_id << std::dec
            << "\n";

  if (state_ == State::kHandshake) {
    if (packet_id != kPktHandshakeId) return false;

    // Handshake packet

    int32_t protocol_version = packet.ReadVarInt();
    std::string server_address = packet.ReadVarString();
    uint16_t server_port = packet.ReadUnsignedShort();
    auto next_state = State{packet.ReadVarInt()};

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
    switch (packet_id) {
      case kPktLoginStartId:
        return HandlePacketLoginStart(packet);
      default:
        std::cerr << "Unknown login packet " << std::hex << packet_id
                  << std::dec << "\n";
        return false;
    }
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

void Protocol_1_16::SendPacket(const Packet &packet) {
  size_t buffer_size = util::kMaxVarIntBytes + packet.Data().size();
  auto buffer = new char[buffer_size];

  auto data = std::unique_ptr<char[]>(buffer);

  auto pkt_len_size_bytes =
      util::WriteVarInt(static_cast<int32_t>(packet.Data().size()),
                        std::as_writable_bytes(std::span{buffer, buffer_size}));
  // TODO: Handle error

  std::memcpy(data.get() + pkt_len_size_bytes, packet.Data().data(),
              packet.Data().size());
  socket_.write(
      std::move(data),
      static_cast<unsigned int>(pkt_len_size_bytes + packet.Data().size()));
}

bool Protocol_1_16::HandlePacketStatusPing(Packet &packet) {
  int64_t payload = packet.ReadLong();

  SendPacketStatusPong(payload);
  return true;
}

bool Protocol_1_16::HandlePacketStatusRequest(Packet &) {
  SendPacketStatusResponse(69, 420, kServerDescription);

  return true;
}

bool Protocol_1_16::HandlePacketLoginStart(Packet &) {
  SendDisconnect(kDisconnectText);
  return true;
}

void Protocol_1_16::SendPacketStatusPong(int64_t payload) {
  Packet packet;
  packet.WriteVarInt(kPktStatusPongId);
  packet.WriteLong(payload);

  SendPacket(packet);
}

void Protocol_1_16::SendPacketStatusResponse(int online_players,
                                             int max_players,
                                             std::u8string_view description) {
  using nlohmann::json;

  auto root = json::object(
      {{"version", json::object({{"name", util::ToMultibyte(kVersionString)},
                                 {"protocol", kProtocolVersion}})},
       {"players", json::object({{"max", max_players},
                                 {"online", online_players},
                                 {"sample", json::array()}})},
       {"description",
        json::object({{"text", util::ToMultibyte(description)}})}});

  Packet packet;
  packet.WriteVarInt(kPktStatusResponseId);
  packet.WriteVarString(util::ToUtf8(root.dump()));

  SendPacket(packet);
}

void Protocol_1_16::SendDisconnect(std::u8string_view reason) {
  using nlohmann::json;

  auto root = json::object({{"text", util::ToMultibyte(reason)}});

  Packet packet;
  packet.WriteVarInt(kPktLoginDisconnectId);
  packet.WriteVarString(util::ToUtf8(root.dump()));

  SendPacket(packet);
}

}  // namespace vega::server::protocol

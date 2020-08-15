#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

#include "uvw.hpp"
#include "vega/server/protocol/packet.h"
#include "vega/server/protocol/protocol.h"

namespace vega::server::protocol {

class Protocol_1_16 : public Protocol {
 public:
  constexpr static int32_t kProtocolVersion = 751;

  enum class State { kHandshake = 0, kStatus = 1, kLogin = 2, kPlay = 3 };

  explicit Protocol_1_16(uvw::TCPHandle &socket);
  ~Protocol_1_16() override;

  virtual bool HandlePacket(Packet packet);
  virtual void SendPacket(Packet packet);

  virtual bool HandlePacketStatusPing(Packet packet);
  virtual bool HandlePacketStatusRequest(Packet packet);

  virtual bool HandlePacketLoginStart(Packet packet);

  virtual void SendPacketStatusPong(int64_t payload);
  virtual void SendPacketStatusResponse(std::u8string_view response);

  void SendDisconnect(std::u8string_view reason) override;

 private:
  uvw::TCPHandle &socket_;
  State state_ = State::kHandshake;
};

}  // namespace vega::server::protocol

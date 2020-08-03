#pragma once

#include <cstdint>
#include <string>

namespace vega {
namespace protocol {

namespace serverbound {

struct HandshakePacket {
  enum class State {
    kStatus = 1,
    kLogin = 2,
  };

  int32_t ProtocolVersion;
  std::string ServerAddress;
  uint16_t ServerPort;
  State NextState;
};

}  // namespace serverbound

}  // namespace protocol
}  // namespace vega

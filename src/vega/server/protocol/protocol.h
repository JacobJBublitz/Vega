#pragma once

namespace vega::server::protocol {

class Protocol {
 public:
  virtual ~Protocol() = default;

  virtual void SendDisconnect(std::u8string_view reason) = 0;
};

}  // namespace vega::server::protocol

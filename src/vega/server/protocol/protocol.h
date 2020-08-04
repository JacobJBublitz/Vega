#pragma once

namespace vega::server::protocol {

class Protocol {
 public:
  virtual ~Protocol() = default;

  virtual void SendDisconnect() = 0;
};

}  // namespace vega::server::protocol

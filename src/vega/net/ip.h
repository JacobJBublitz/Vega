#pragma once

#include <array>
#include <cstddef>
#include <ostream>
#include <span>

namespace vega::net {

struct Ipv4Address {
 public:
  Ipv4Address(std::span<std::byte, 4> data)
      : data_({data[0], data[1], data[2], data[3]}) {}

 private:
  std::array<std::byte, 4> data_;
};
static_assert(sizeof(Ipv4Address) == 4);

class Ipv6Address {
 private:
  std::array<std::byte, 16> data_;
};
static_assert(sizeof(Ipv6Address) == 16);

}  // namespace vega::net

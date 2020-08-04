#include "packet.h"

namespace vega::server::protocol {

Packet::Packet() {}

Packet::Packet(std::span<const std::byte> data) {
  data_.resize(data.size());
  std::copy(data.begin(), data.end(), data_.begin());
}

void Packet::ReadBytes(std::span<std::byte> buffer) {
  // Check how bytes we can read
  auto avail_bytes = data_.size() - offset_;

  // Find out the amount of bytes we can copy
  auto num_copied_bytes = std::min(avail_bytes, buffer.size());

  // Copy bytes and increment offset
  std::copy(data_.begin() + offset_, data_.begin() + offset_ + num_copied_bytes,
            buffer.begin());
  offset_ += num_copied_bytes;
}

void Packet::WriteBytes(std::span<const std::byte> buffer) {
  // Find out how many bytes we can write
  auto writable_bytes = data_.size() - offset_;

  if (writable_bytes < buffer.size_bytes()) {
    // Not enough room to just write bytes. Allocate remaining bytes
    data_.resize(data_.size() + buffer.size_bytes() - writable_bytes);
  }

  std::copy(buffer.begin(), buffer.end(), data_.begin() + offset_);
  offset_ += buffer.size_bytes();
}

}  // namespace vega::server::protocol
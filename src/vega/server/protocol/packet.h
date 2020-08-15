#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <string>
#include <vector>

#include "vega/util/varint.h"

namespace vega::server::protocol {

class Packet {
 public:
  constexpr static size_t kMaxVarIntBytes = 5;
  constexpr static size_t kMaxVarLongBytes = 10;

  Packet();
  explicit Packet(std::span<const std::byte> data);

  inline std::byte ReadByte() { return ReadBytes<1>().front(); }

  void ReadBytes(std::span<std::byte> buffer);

  template <size_t S>
  std::array<std::byte, S> ReadBytes() {
    std::array<std::byte, S> buffer{};
    ReadBytes(buffer);
    return buffer;
  }

  inline int64_t ReadLong() {
    auto buffer = ReadBytes<sizeof(int64_t)>();

    // Swap byte order if the CPU is not big-endian
    // Probably not the most efficient thing in the world but it works
    if constexpr (std::endian::native != std::endian::big)
      std::reverse(buffer.begin(), buffer.end());

    // TODO: Memory access may be unaligned. Use bitwise operations instead
    return *reinterpret_cast<int64_t*>(buffer.data());
  }

  inline uint16_t ReadUnsignedShort() {
    auto buffer = ReadBytes<sizeof(uint16_t)>();

    // Swap byte order if the CPU is not big-endian
    // Probably not the most efficient thing in the world but it works
    if constexpr (std::endian::native != std::endian::big)
      std::reverse(buffer.begin(), buffer.end());

    // TODO: Memory access may be unaligned. Use bitwise operations instead
    return *reinterpret_cast<uint16_t*>(buffer.data());
  }

  inline int32_t ReadVarInt() {
    auto res = util::ReadVarInt(Data().subspan(offset_));
    // TODO: Handle error
    offset_ += res.second;
    return res.first;
  }

  inline int64_t ReadVarLong() {
    auto res = util::ReadVarLong(Data().subspan(offset_));
    // TODO: Handle error
    offset_ += res.second;
    return res.first;
  }

  inline std::string ReadVarString() {
    int32_t len = ReadVarInt();
    std::vector<std::byte> str_data(len);
    ReadBytes(str_data);

    return {reinterpret_cast<const char*>(str_data.data()),
            static_cast<size_t>(len)};
  }

  void WriteBytes(std::span<const std::byte> bytes);

  inline void WriteLong(int64_t value) {
    std::array<std::byte, sizeof(value)> buffer{};

    std::memcpy(buffer.data(), &value, sizeof(value));

    // Swap byte order if the CPU is not big endian
    // Probably not the most efficient thing in the world but it works
    if constexpr (std::endian::native != std::endian::big)
      std::reverse(buffer.begin(), buffer.end());

    WriteBytes(buffer);
  }

  inline void WriteVarInt(int32_t value) {
    std::array<std::byte, kMaxVarIntBytes> buffer{};

    auto uvalue = static_cast<uint32_t>(value);
    size_t byte_count;
    for (byte_count = 0; byte_count < buffer.size(); ++byte_count) {
      auto b = std::byte(uvalue & 0x7fu);

      uvalue >>= 7u;
      if (uvalue != 0) b |= std::byte{0x80};

      buffer[byte_count] = b;

      if ((b & std::byte{0x80}) == std::byte(0x00)) break;
    }

    WriteBytes(
        static_cast<std::span<std::byte>>(buffer).subspan(0, byte_count + 1));
  }

  inline void WriteVarLong(int64_t value) {
    std::array<std::byte, kMaxVarLongBytes> buffer{};

    auto uvalue = static_cast<uint64_t>(value);
    size_t byte_count;
    for (byte_count = 0; byte_count < buffer.size(); ++byte_count) {
      auto b = std::byte(uvalue & 0x7fu);

      uvalue >>= 7u;
      if (uvalue != 0) b |= std::byte{0x80};

      buffer[byte_count] = b;

      if ((b & std::byte{0x80}) == std::byte(0x00)) break;
    }

    WriteBytes(
        static_cast<std::span<std::byte>>(buffer).subspan(0, byte_count + 1));
  }

  inline void WriteVarString(std::u8string_view value) {
    WriteVarInt(static_cast<int32_t>(value.size()));
    WriteBytes(std::as_bytes(static_cast<std::span<const char8_t>>(value)));
  }

  [[nodiscard]] constexpr std::span<const std::byte> Data() const noexcept {
    return data_;
  }

 private:
  std::vector<std::byte> data_;
  size_t offset_ = 0;
};

}  // namespace vega::server::protocol

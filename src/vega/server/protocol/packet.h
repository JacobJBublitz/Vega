#pragma once

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace vega::server::protocol {

class Packet {
 public:
  constexpr static size_t kMaxVarIntBytes = 5;
  constexpr static size_t kMaxVarLongBytes = 10;

  Packet();
  Packet(std::span<const std::byte> data);

  inline std::byte ReadByte() { return ReadBytes<1>().front(); }

  void ReadBytes(std::span<std::byte> buffer);

  template <size_t S>
  std::array<std::byte, S> ReadBytes() {
    std::array<std::byte, S> buffer;
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
    int32_t value = 0;
    for (size_t i = 0; i < kMaxVarIntBytes; ++i) {
      auto b = ReadByte();
      value = (value << 7) | (std::to_integer<int32_t>(b) & 0x7f);

      if ((b & std::byte{0x80}) != std::byte{0}) break;
    }

    return value;
  }

  inline int64_t ReadVarLong() {
    int64_t value = 0;
    for (size_t i = 0; i < kMaxVarLongBytes; ++i) {
      auto b = ReadByte();
      value = (value << 7) | (std::to_integer<int32_t>(b) & 0x7f);

      if ((b & std::byte{0x80}) != std::byte{0}) break;
    }

    return value;
  }

  inline std::string ReadVarString() {
    int32_t len = ReadVarInt();
    std::vector<std::byte> str_data;
    ReadBytes(str_data);

    return {reinterpret_cast<const char*>(str_data.data()),
            static_cast<size_t>(len)};
  }

  void WriteBytes(std::span<const std::byte> bytes);

  inline void WriteLong(int64_t value) {
    std::array<std::byte, sizeof(value)> buffer;

    std::memcpy(buffer.data(), &value, sizeof(value));

    // Swap byte order if the CPU is not big endian
    // Probably not the most efficient thing in the world but it works
    if constexpr (std::endian::native != std::endian::big)
      std::reverse(buffer.begin(), buffer.end());

    WriteBytes(buffer);
  }

  void WriteVarInt(int32_t value) {
    std::array<std::byte, kMaxVarIntBytes> buffer;

    size_t byte_count;
    for (byte_count = 0; byte_count < buffer.size(); ++byte_count) {
      std::byte b = std::byte{value & 0x7f};

      value >>= 7;
      if (value != 0) b |= std::byte{0x80};

      buffer[byte_count] = b;
    }

    WriteBytes(
        static_cast<std::span<std::byte>>(buffer).subspan(0, byte_count));
  }

  void WriteVarLong(int64_t value) {
    std::array<std::byte, kMaxVarLongBytes> buffer;

    size_t byte_count;
    for (byte_count = 0; byte_count < buffer.size(); ++byte_count) {
      std::byte b = std::byte{value & 0x7f};

      value >>= 7;
      if (value != 0) b |= std::byte{0x80};

      buffer[byte_count] = b;
    }

    WriteBytes(
        static_cast<std::span<std::byte>>(buffer).subspan(0, byte_count));
  }

  inline void WriteVarString(std::u8string_view value) {
    WriteVarInt(static_cast<int32_t>(value.size()));
    WriteBytes(std::as_bytes(static_cast<std::span<const char8_t>>(value)));
  }

  constexpr std::span<const std::byte> Data() const noexcept { return data_; }

 private:
  std::vector<std::byte> data_;
  size_t offset_;
};

}  // namespace vega::server::protocol

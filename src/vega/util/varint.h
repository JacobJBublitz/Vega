#pragma once

#include <span>
#include <utility>

namespace vega::util {

enum class VarIntError {
  kInvalid,
  kBufferTooSmall,
};

namespace detail {

template <size_t Extent, class Type, size_t MaxBytes>
constexpr std::pair<Type, size_t> ReadVarNumber(
    std::span<const std::byte, Extent> buffer) noexcept {
  Type result = 0;
  size_t bytes_read = 0;
  uint8_t value;
  do {
    if (bytes_read >= buffer.size())
      return {static_cast<Type>(VarIntError::kBufferTooSmall), 0};
    if (bytes_read > MaxBytes)
      return {static_cast<Type>(VarIntError::kInvalid), 0};

    value = std::to_integer<uint8_t>(buffer[bytes_read++]);
    result |= (value & 0x7fu) << (7 * (bytes_read - 1));
  } while ((value & 0x80u) != 0);

  return {result, bytes_read};
}

template <size_t Extent, class Type>
constexpr size_t WriteVarNumber(Type value,
                                std::span<std::byte, Extent> buffer) noexcept {
  auto uvalue = static_cast<std::make_unsigned_t<Type>>(value);

  size_t written_bytes = 0;
  do {
    if (written_bytes >= buffer.size()) return 0;

    auto temp = std::byte(uvalue & 0x7f);
    uvalue >>= 7;
    if (uvalue != 0) temp |= std::byte{0x80};

    buffer[written_bytes++] = temp;
  } while (uvalue != 0);

  return written_bytes;
}

}  // namespace detail

constexpr size_t kMaxVarIntBytes = 5;
constexpr size_t kMaxVarLongBytes = 10;

/**
 * @brief Reads a VarInt from a buffer.
 *
 * If the VarInt is invalid (i.e. The amount of bytes used to represent it is
 * greater than `kMaxVarIntBytes`) a `VarIntError::kInvalid` is returned.
 *
 * If the buffer is not large enough to fully read the VarInt a
 * `VarIntError::kBufferTooSmall` is returned.
 *
 * @param buffer The buffer the varint is stored in.
 * @return A pair containing the value and the amount of bytes read from the
 * buffer. If an error occurs, the amount of bytes read is zero and the value is
 * a `VarIntError` describing the error.
 */
template <size_t Extent>
constexpr std::pair<int32_t, size_t> ReadVarInt(
    std::span<const std::byte, Extent> buffer) noexcept {
  return detail::ReadVarNumber<Extent, int32_t, kMaxVarIntBytes>(buffer);
}

template <size_t Extent>
constexpr std::pair<int64_t, size_t> ReadVarLong(
    std::span<const std::byte, Extent> buffer) noexcept {
  return detail::ReadVarNumber<Extent, int64_t, kMaxVarLongBytes>(buffer);
}

template <size_t Extent>
constexpr size_t WriteVarInt(int32_t value,
                             std::span<std::byte, Extent> buffer) noexcept {
  return detail::WriteVarNumber<Extent, int32_t>(value, buffer);
}

template <size_t Extent>
constexpr size_t WriteVarLong(int64_t value,
                              std::span<std::byte, Extent> buffer) noexcept {
  return detail::WriteVarNumber<Extent, int64_t>(value, buffer);
}

}  // namespace vega::util

#pragma once

#include <array>

namespace vega {
namespace util {

/**
 * The maximum size in bytes of a VarInt
 */
constexpr size_t kMaxVarIntBytes = 5;

/**
 * The maxium size in bytes of a VarLong
 */
constexpr size_t kMaxVarLongBytes = 10;

/**
 * Reads a VarInt from a buffer.
 *
 * On error out_num_read is set to 0.
 */
inline int32_t FromVarInt(uint8_t *buffer, size_t buffer_size,
                          size_t *out_num_read = nullptr) {
  int32_t result = 0;
  size_t num_read = 0;

  for (num_read = 0; num_read < buffer_size; ++num_read) {
    if (num_read > kMaxVarIntBytes) {
      // If we read too many bytes set the number of bytes read to 0 to indicate
      // an error and return the current result
      *out_num_read = 0;
      return result;
    }

    uint8_t byte = buffer[num_read];

    int32_t value = byte & 0b01111111;
    result |= value << (7 * num_read);

    if ((byte & 0b10000000) == 0) break;
  }

  *out_num_read = num_read;
  return result;
}

/**
 * Writes an integer to a buffer as a VarInt.
 *
 * If buffer_size is zero the buffer will not be accessed and no error will
 * occur. This behavior can be used to determine the amount of bytes the value
 * will take up when written to a buffer.
 *
 * Returns the amount of bytes written to the buffer.
 * On error zero is returned and the buffer may be modified.
 *
 * An error will occur if the buffer is not large enough to store the full
 * integer value.
 */
inline size_t ToVarInt(int32_t value, uint8_t *buffer, size_t buffer_size) {
  size_t written_bytes = 0;
  do {
    uint8_t temp = static_cast<uint8_t>(value & 0b01111111);

    // Unsigned shift here so we don't copy the sign bit
    value = static_cast<uint32_t>(value) >> 7;

    // If the value is not zero set the 8th bit to signify we have more bytes
    // following
    if (value != 0) temp |= 0b10000000;

    // Check for a buffer size of zero here because we ignore the buffer if the
    // size is zero
    if (buffer_size != 0) {
      if (written_bytes < buffer_size) {
        // Write the byte if we have room
        buffer[written_bytes] = temp;
      } else {
        // Otherwise return an error
        return 0;
      }
    }
    written_bytes++;
  } while (value != 0);

  return written_bytes;
}

/**
 * Writes an integer to a buffer as a VarLong.
 *
 * If buffer_size is zero the buffer will not be accessed and no error will
 * occur. This behavior can be used to determine the amount of bytes the value
 * will take up when written to a buffer.
 *
 * Returns the amount of bytes written to the buffer.
 * On error zero is returned and the buffer may be modified.
 *
 * An error will occur if the buffer is not large enough to store the full
 * integer value.
 */
inline size_t ToVarLong(int64_t value, uint8_t *buffer, size_t buffer_size) {
  size_t written_bytes = 0;
  do {
    uint8_t temp = static_cast<uint8_t>(value & 0b01111111);

    // Unsigned shift here so we don't copy the sign bit
    value = static_cast<uint64_t>(value) >> 7;

    // If the value is not zero set the 8th bit to signify we have more bytes
    // following
    if (value != 0) temp |= 0b10000000;

    // Check for a buffer size of zero here because we ignore the buffer if the
    // size is zero
    if (buffer_size != 0) {
      if (written_bytes < buffer_size) {
        // Write the byte if we have room
        buffer[written_bytes] = temp;
      } else {
        // Otherwise return an error
        return 0;
      }
    }
    written_bytes++;
  } while (value != 0);

  return written_bytes;
}

}  // namespace util
}  // namespace vega

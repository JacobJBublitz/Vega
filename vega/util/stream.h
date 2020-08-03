#pragma once

#include <cstdint>
#include <vector>

namespace vega {
namespace util {

class Stream {
 public:
  virtual ~Stream() = default;

  virtual size_t Read(void *buffer, size_t buffer_size) = 0;

  template <class T>
  size_t Read(std::vector<T> &buffer) {
    return Read(buffer.data(), buffer.size() * sizeof(T));
  }

  virtual size_t Write(const void *buffer, size_t buffer_size) = 0;

  template <class T>
  size_t Write(const std::vector<T> &buffer) {
    return Write(buffer.data(), buffer.size() * sizeof(T));
  }
};

template <class T, class A>
size_t operator<<(Stream &stream, const std::vector<T, A> &buffer) {
  return stream.Write(buffer.data(), buffer.size() * sizeof(T));
}

template <class T, class A>
size_t operator>>(Stream &stream, std::vector<T, A> &buffer) {
  return stream.Read(buffer.data(), buffer.size() & sizeof(T));
}

}  // namespace util
}  // namespace vega

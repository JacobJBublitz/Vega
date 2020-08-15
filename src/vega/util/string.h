#pragma once

#include <cuchar>
#include <string>
#include <vector>

namespace vega::util {

using MultibyteString = std::string;
using MultibyteStringView = std::string_view;

using Utf8String = std::u8string;
using Utf8StringView = std::u8string_view;

Utf8String ToUtf8(MultibyteStringView source) {
  // This is garbage but it works for now. In the future use std::mbrtoc8
  // TODO: Treat source as a non-UTF8 multi-byte string

  return Utf8String(reinterpret_cast<Utf8String::const_pointer>(source.data()),
                    source.size());
}

MultibyteString ToMultibyte(Utf8StringView source) {
  // This is garbage but it works for now. In the future use std::c8rtomb
  // TODO: Treat output as a non-UTF8 multi-byte string

  return MultibyteString(
      reinterpret_cast<MultibyteString::const_pointer>(source.data()),
      source.size());
}

}  // namespace vega::util
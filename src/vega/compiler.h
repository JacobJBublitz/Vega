#pragma once

namespace vega {

enum class Compiler { kUnknown, kClang, kGcc, kMsvc };

enum class CpuArchitecture { kAmd64, kIntel32 };

enum class OperatingSystem { kLinux, kMacOs, kWindows };

#if defined(__clang__)
constexpr Compiler kCompiler = Compiler::kClang;
#define VEGA_COMPILER_CLANG 1
#elif defined(__GNUC__)
constexpr Compiler kCompiler = Compiler::kGcc;
#define VEGA_COMPILER_GCC 1
#elif defined(_MSC_VER)
constexpr Compiler kCompiler = Compiler::kMsvc;
#define VEGA_COMPILER_MSVC 1
#else
constexpr Compiler kCompiler = Compiler::kUnknown;
#endif

#if defined(__amd64__) || defined(_M_AMD64)
constexpr CpuArchitecture kArchitecture = CpuArchitecture::kAmd64;
#define VEGA_ARCH_AMD64 1
#elif defined(__i386__) || defined(_M_IX86)
constexpr CpuArchitecture kArchitecture = CpuArchitecture::kIntel32;
#define VEGA_ARCH_INTEL32 1
#endif

#if defined(__linux__)
constexpr OperatingSystem kOperatingSystem = OperatingSystem::kLinux;
#define VEGA_OS_LINUX 1
#elif defined(__APPLE__) && defined(__MACH__)
constexpr OperatingSystem kOperatingSystem = OperatingSystem::kMacOs;
#define VEGA_OS_MACOS 1
#elif defined(_WIN32)
constexpr OperatingSystem kOperatingSystem = OperatingSystem::kWindows;
#define VEGA_OS_WINDOWS 1
#else
#error Unable to determine target operating system. Make sure a supported compiler is being used or update vega/compiler.h
#endif

}  // namespace vega
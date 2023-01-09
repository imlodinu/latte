#pragma once

#include <EABase/eabase.h>
#include <cstdint>
#include <cstdlib>

#if EA_PLATFORM_WINDOWS
#if LATTE_EXPORTS
#define LATTE_API __declspec(dllexport)
#else
#define LATTE_API __declspec(dllimport)
#endif
#endif

#include "math.h"

namespace latte {
class LATTE_API NonCopyable {
public:
  NonCopyable(const NonCopyable &) = delete;
  NonCopyable &operator=(const NonCopyable &) = delete;

  virtual ~NonCopyable() = default;

protected:
  NonCopyable() = default;
};
} // namespace latte

#if EA_PLATFORM_WINDOWS
#define LATTE_SYSTEM_WINDOWS 1
#elif EA_PLATFORM_LINUX
#define LATTE_SYSTEM_LINUX 1
#elif EA_PLATFORM_OSX
#define LATTE_SYSTEM_OSX 1
#elif EA_PLATFORM_ANDROID
#define LATTE_SYSTEM_ANDROID 1
#elif EA_PLATFORM_IOS
#define LATTE_SYSTEM_IOS 1
#endif

#include <gsl/gsl>
#if !(__cplusplus >= 202002L)
namespace std {
using namespace gsl;
} // namespace std
#else
#include <span>
#endif
#pragma once

#include "latte/prelude.h"

#ifdef LATTE_SYSTEM_WINDOWS
#define UNICODE
#define _UNICODE

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace latte::platform {
class Window;
class Display;
#ifdef LATTE_SYSTEM_WINDOWS
using NativeWindowHandle = HWND;
using NativeDisplayHandle = HMONITOR;
#elif defined(LATTE_SYSTEM_LINUX)
using NativeWindowHandle = void *;
using NativeDisplayHandle = void *;
#elif defined(LATTE_SYSTEM_OSX)
using NativeWindowHandle = void *;
using NativeDisplayHandle = void *;
#endif
} // namespace latte::platform
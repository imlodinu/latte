#pragma once
// meant to be only included once ub ../window.cpp

#include "latte/platform/display.h"
#include "latte/platform/window.h"

#include <shellapi.h>
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC ((USHORT)0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE ((USHORT)0x02)
#endif

using namespace latte::platform;
class Win32Window;

static Win32Window *GetWindowFromUserData(HWND window) {
  return reinterpret_cast<Win32Window *>(
      GetWindowLongPtrW(window, GWLP_USERDATA));
}
static LRESULT CALLBACK Win32WindowCallback(HWND window, UINT message,
                                            WPARAM wparam, LPARAM lparam);

class Win32WindowClass {
public:
  Win32WindowClass(Win32WindowClass const &) = delete;
  Win32WindowClass(Win32WindowClass &&) = delete;

  ~Win32WindowClass() {
    UnregisterClassW(ClassName(), GetModuleHandleW(nullptr));
  }

  static Win32WindowClass *Get() {
    static Win32WindowClass instance;
    return &instance;
  }
  [[nodiscard]] static const wchar_t *ClassName() {
    static wchar_t class_name[] = L"latte_window!";
    return class_name;
  }

private:
  Win32WindowClass() {
    WNDCLASSEX wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
    wcex.lpfnWndProc = Win32WindowCallback;
    wcex.hInstance = GetModuleHandleW(nullptr);
#if EA_PROCESSOR_ARM64
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW);
#else
    wcex.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
#endif
    wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wcex.lpszClassName = ClassName();
    wcex.hIconSm = LoadIconW(nullptr, IDI_APPLICATION);
    auto register_result = RegisterClassExW(&wcex);
    // ASSERT(register_result, "Failed to register window class");
  }
};

struct WindowAppearance {
  DWORD style = 0;
  uint32_t2 position{};
  uint32_t2 size{};
};

static void SetUserData(HWND window, void *user_data) {
  SetWindowLongPtr(window, GWLP_USERDATA,
                   reinterpret_cast<LONG_PTR>(user_data));
}

static RECT GetClientArea(LONG width, LONG height, DWORD style) {
  RECT client_area{0, 0, width, height};
  AdjustWindowRect(&client_area, style, FALSE);
  return client_area;
}

static DWORD GetWindowStyle(const WindowDescriptor &desc) {
  DWORD style = (WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

  if (desc.borderless) {
    style |= WS_POPUP;
  } else {
    style |= (WS_SYSMENU | WS_MINIMIZEBOX | WS_CAPTION);
    if (desc.resizable) {
      style |= (WS_SIZEBOX | WS_MAXIMIZEBOX);
    }
  }

  if (desc.visible) {
    style |= WS_VISIBLE;
  }

  if (desc.accept_drop_files) {
    style |= WM_DROPFILES;
  }

  return style;
}

static WindowAppearance GetWindowAppearance(const WindowDescriptor &desc) {
  WindowAppearance appearance{0};
  appearance.style = GetWindowStyle(desc);
  auto rect_client_area =
      GetClientArea(static_cast<LONG>(desc.size.x),
                    static_cast<LONG>(desc.size.y), appearance.style);

  appearance.size.x =
      static_cast<uint32_t>(rect_client_area.right - rect_client_area.left);
  appearance.size.y =
      static_cast<uint32_t>(rect_client_area.bottom - rect_client_area.top);
  appearance.position = desc.position;
  return appearance;
}

std::wstring ToWideString(const std::string &str) {
  uint32_t size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
  std::wstring result(size, 0);
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, result.data(), size);
  return result;
}

std::string ToNarrowString(const std::wstring &str) {
  uint32_t size = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, nullptr, 0,
                                      nullptr, nullptr);
  std::string result(size, 0);
  WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, result.data(), size, nullptr,
                      nullptr);
  return result;
}

class Win32Window final : public Window {
public:
  Win32Window(const WindowDescriptor &desc) { CreateWindowHandle(desc); }
  virtual ~Win32Window() {
    if (m_window != nullptr) {
      DestroyWindow(m_window);
    }
  }

  virtual void SetPosition(const uint32_t2 &position) override final {
    SetWindowPos(m_window, nullptr, position.x, position.y, 0, 0,
                 SWP_NOZORDER | SWP_NOSIZE);
  }
  virtual void SetTitle(const std::string &title) override final {
    SetWindowTextW(m_window, ToWideString(title).c_str());
  }
  virtual void SetShown(bool shown) override final {
    ShowWindow(m_window, shown ? SW_SHOW : SW_HIDE);
  }
  virtual void SetSize(const uint32_t2 &size,
                       bool use_client_area = true) override final {
    uint32_t window_x;
    uint32_t window_y;
    if (use_client_area) {
      auto rect_client_area =
          GetClientArea(size.x, size.y, GetWindowLongW(m_window, GWL_STYLE));
      window_x = rect_client_area.right - rect_client_area.left;
      window_y = rect_client_area.bottom - rect_client_area.top;
    } else {
      window_x = size.x;
      window_y = size.y;
    }
    SetWindowPos(m_window, HWND_TOP, 0, 0, window_x, window_y,
                 SWP_NOZORDER | SWP_NOMOVE);
  }
  virtual void SetDescriptor(const WindowDescriptor &desc) override final {
    LONG style_flags = GetWindowLongW(m_window, GWL_STYLE);

    bool borderless = ((style_flags & WS_CAPTION) == 0);
    bool resizable = ((style_flags & WS_SIZEBOX) != 0);

    DWORD new_style_flags = GetWindowStyle(desc);
    if ((style_flags & WS_MAXIMIZE) != 0) {
      new_style_flags |= WS_MAXIMIZE;
    }
    if ((style_flags & WS_MINIMIZE) != 0) {
      new_style_flags |= WS_MINIMIZE;
    }
    m_window_style = style_flags;
    bool changed = style_flags != new_style_flags;
    uint32_t2 size = GetSize();
    uint32_t2 position = GetPosition();

    bool position_changed = (static_cast<uint32_t>(desc.position.x) !=
                                 static_cast<uint32_t>(position.x) ||
                             static_cast<uint32_t>(desc.position.y) !=
                                 static_cast<uint32_t>(position.y));
    bool size_changed =
        (static_cast<uint32_t>(desc.size.x) != static_cast<uint32_t>(size.x) ||
         static_cast<uint32_t>(desc.size.y) != static_cast<uint32_t>(size.y));

    if (changed || position_changed || size_changed) {
      UINT style = SWP_NOZORDER;
      if (changed) {
        ShowWindow(m_window, SW_HIDE);
        SetWindowLongPtrW(m_window, GWL_STYLE, new_style_flags);
        style |= SWP_FRAMECHANGED;
      }
      WindowAppearance appearance = GetWindowAppearance(desc);
      if (desc.visible) {
        style |= SWP_SHOWWINDOW;
      }
      if ((new_style_flags & WS_MAXIMIZE) != 0) {
        style |= (SWP_NOSIZE | SWP_NOMOVE);
      }
      if (borderless == desc.borderless && resizable == desc.resizable) {
        if (!position_changed) {
          style |= SWP_NOMOVE;
        }
        if (!size_changed) {
          style |= SWP_NOSIZE;
        }
      }
      SetWindowPos(m_window,
                   0, // ignore, due to SWP_NOZORDER flag
                   appearance.position.x, appearance.position.y,
                   static_cast<uint32_t>(appearance.size.x),
                   static_cast<uint32_t>(appearance.size.y), style);
    }
  };
  virtual void SetFocus() override final { BringWindowToTop(m_window); }
  virtual void SetFullscreen(bool fullscreen) override final {
    DWORD window_style = GetWindowLongW(m_window, GWL_STYLE);
    if (fullscreen == m_fullscreen) {
      return;
    }
    m_fullscreen = fullscreen;
    if (fullscreen) {
      MONITORINFO monitor_info = {sizeof(monitor_info)};

      if ((GetWindowPlacement(m_window, &m_previous_window_placement) != 0) &&
          (GetMonitorInfoW(
               MonitorFromWindow(m_window, MONITOR_DEFAULTTOPRIMARY),
               &monitor_info) != 0)) {
        SetWindowLongW(m_window, GWL_STYLE,
                       window_style & ~WS_OVERLAPPEDWINDOW);
        SetWindowPos(m_window, HWND_TOP, monitor_info.rcMonitor.left,
                     monitor_info.rcMonitor.top,
                     monitor_info.rcMonitor.right - monitor_info.rcMonitor.left,
                     monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
      }
    } else {
      SetWindowLong(m_window, GWL_STYLE, m_window_style);
      SetWindowPlacement(m_window, &m_previous_window_placement);
      SetWindowPos(m_window, NULL, 0, 0, 0, 0,
                   SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                       SWP_FRAMECHANGED);
    }
  }

  virtual uint32_t2 GetPosition() const override final {
    RECT rect;
    GetWindowRect(m_window, &rect);
    MapWindowPoints(HWND_DESKTOP, GetParent(m_window),
                    reinterpret_cast<LPPOINT>(&rect), 2);
    return uint32_t2{static_cast<uint32_t>(rect.left),
                     static_cast<uint32_t>(rect.top)};
  }
  virtual std::string GetTitle() const override final {
    uint32_t title_len = GetWindowTextLengthW(m_window);
    if (title_len == 0) {
      return {};
    }
    std::wstring title;
    title.resize(title_len + 1);
    GetWindowTextW(m_window, title.data(), static_cast<uint32_t>(title.size()));
    return ToNarrowString(title);
  }
  virtual bool IsShown() const override final {
    return IsWindowVisible(m_window) != FALSE;
  }
  virtual uint32_t2 GetSize(bool use_client_area = true) const override final {
    RECT rect;
    if (use_client_area) {
      GetClientRect(m_window, &rect);
    } else {
      GetWindowRect(m_window, &rect);
    }
    return uint32_t2{static_cast<uint32_t>(rect.right - rect.left),
                     static_cast<uint32_t>(rect.bottom - rect.top)};
  }
  virtual WindowDescriptor GetDescriptor() const override final {
    LONG style_flags = GetWindowLongW(m_window, GWL_STYLE);
    uint32_t2 size = GetSize();

    WindowDescriptor desc;
    desc.title = GetTitle();
    desc.position = GetPosition();
    desc.size = size;

    desc.visible = ((style_flags & WS_VISIBLE) != 0);
    desc.borderless = ((style_flags & WS_CAPTION) == 0);
    desc.resizable = ((style_flags & WS_SIZEBOX) != 0);
    desc.accept_drop_files = ((style_flags & WM_DROPFILES) != 0);

    return desc;
  }
  virtual bool HasFocus() const override final {
    return GetFocus() == m_window;
  }
  virtual bool IsFullscreen() const override final { return m_fullscreen; }

  virtual Display *GetResidentDisplay() const override final {
    std::span<Display *> displays = Display::GetList();
    if (displays.empty()) {
      return nullptr;
    }
    HMONITOR monitor = MonitorFromWindow(m_window, MONITOR_DEFAULTTONEAREST);
    for (const Display *display : displays) {
      if (display->GetNativeHandle() == monitor) {
        return const_cast<Display *>(display);
      }
    }
    return nullptr;
  }
  virtual NativeWindowHandle GetNativeHandle() const override final {
    return m_window;
  }

protected:
  virtual void OnProcessEvents() override final {
    MSG msg;
    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE) != FALSE) {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
  };

private:
  HWND m_window = nullptr;
  WINDOWPLACEMENT m_previous_window_placement = {sizeof(WINDOWPLACEMENT)};
  uint32_t m_window_style = 0;
  bool m_fullscreen = false;

  void CreateWindowHandle(const WindowDescriptor &desc) {
    // Make sure it's initialized
    Win32WindowClass::Get();
    WindowAppearance appearance = GetWindowAppearance(desc);
    m_window_style = appearance.style;
    HWND window = CreateWindowExW(
        0, Win32WindowClass::ClassName(), ToWideString(desc.title).c_str(),
        appearance.style, appearance.position.x, appearance.position.y,
        appearance.size.x, appearance.size.y, nullptr, nullptr,
        GetModuleHandleW(nullptr), nullptr);
    m_window = window;

    if (desc.visible) {
      ShowWindow(window, SW_SHOW);
    }
#if !(EA_PROCESSOR_ARM64)
    if (desc.accept_drop_files) {
      DragAcceptFiles(window, TRUE);
    }
#endif
    SetUserData(window, this);
  }
};

static LRESULT CALLBACK Win32WindowCallback(HWND window, UINT message,
                                            WPARAM wparam, LPARAM lparam) {
  switch (message) {
  case WM_CREATE: {
    RAWINPUTDEVICE device;
    device.usUsagePage = HID_USAGE_PAGE_GENERIC;
    device.usUsage = HID_USAGE_GENERIC_MOUSE;
    device.dwFlags = RIDEV_INPUTSINK;
    device.hwndTarget = window;

    RegisterRawInputDevices(&device, 1, sizeof(RAWINPUTDEVICE));
  } break;
  case WM_CLOSE:
    if (Win32Window *window_w = GetWindowFromUserData(window)) {
      window_w->PostExit();
    }
    break;
  }
  return DefWindowProcW(window, message, wparam, lparam);
}
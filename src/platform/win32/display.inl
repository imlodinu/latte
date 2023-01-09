#pragma once

#include "latte/platform/display.h"

using namespace latte::platform;
class Win32Display;

struct Win32DisplayContainer {
  Win32DisplayContainer() = default;
  Win32DisplayContainer(std::unique_ptr<Win32Display> &&display,
                        uint32_t cache_index)
      : display{std::move(display)}, cache_index{cache_index} {}

  std::unique_ptr<Win32Display> display{nullptr};
  uint32_t cache_index{0};
};

static std::vector<Win32DisplayContainer> g_display_list;
static std::vector<Win32Display *> g_display_ref;
static Win32Display *g_primary_display{nullptr};
static uint32_t g_display_cache_index{0};

struct MonitorChangedInfo {
  size_t registered_monitors;
  size_t unregistered_monitors;
};

static void Convert(DisplayModeDescriptor &dst, DEVMODE const &src) {
  dst.resolution.x = static_cast<uint32_t>(src.dmPelsWidth);
  dst.resolution.y = static_cast<uint32_t>(src.dmPelsHeight);
  dst.refresh_rate = static_cast<uint32_t>(src.dmDisplayFrequency);
}

static void Convert(DEVMODE &dst, DisplayModeDescriptor const &src) {
  dst.dmPelsWidth = static_cast<DWORD>(src.resolution.x);
  dst.dmPelsHeight = static_cast<DWORD>(src.resolution.y);
  dst.dmDisplayFrequency = static_cast<DWORD>(src.refresh_rate);
}

static BOOL CALLBACK Win32MonitorChangedEnumProcedure(HMONITOR monitor, HDC hDC,
                                                      LPRECT rect, LPARAM data);

static bool HasMonitorListChanged() {
  /* Check if there are any unregistered monitors or if we lost any monitors */
  MonitorChangedInfo info = {0, 0};
  EnumDisplayMonitors(nullptr, nullptr, Win32MonitorChangedEnumProcedure,
                      reinterpret_cast<LPARAM>(&info));
  return (info.unregistered_monitors > 0 ||
          info.registered_monitors != g_display_list.size());
}

static BOOL CALLBACK Win32MonitorEnumProcedure(HMONITOR monitor, HDC hDC,
                                               LPRECT rect, LPARAM data);

static bool UpdateDisplayList() {
  if (HasMonitorListChanged()) {
    /* Clear primary display */
    g_primary_display = nullptr;

    /* Move to next cache index to determine which display entry is outdated */
    g_display_cache_index = (g_display_cache_index + 1) % 2;

    /* Gather new monitors */
    EnumDisplayMonitors(nullptr, nullptr, Win32MonitorEnumProcedure, 0);

    g_display_list.erase(
        std::remove_if(g_display_list.begin(), g_display_list.end(),
                       [](const Win32DisplayContainer &entry) -> bool {
                         return (entry.cache_index != g_display_cache_index);
                       }),
        g_display_list.end());

    return true;
  }
  return false;
}

static auto IsCursorVisible(bool &visible) -> bool {
  CURSORINFO info;
  info.cbSize = sizeof(CURSORINFO);
  if (::GetCursorInfo(&info) != FALSE) {
    visible = ((info.flags & CURSOR_SHOWING) != 0);
    return true;
  }
  return false;
}

size_t MonitorCount() {
  UpdateDisplayList();
  return g_display_list.size();
}

class Win32Display final : public Display {
public:
  Win32Display(HMONITOR monitor) : m_monitor(monitor) {}
  virtual bool IsPrimary() const override final {
    MONITORINFO info;
    GetInfo(info);
    return ((info.dwFlags & MONITORINFOF_PRIMARY) != 0);
  }
  virtual std::string GetDeviceName() const override final {
    MONITORINFOEX infro_ex;
    GetInfo(infro_ex);
    const size_t DEVICE_NAME_LENGTH = 256;
    char device_name[DEVICE_NAME_LENGTH];
    auto device_name_size =
        WideCharToMultiByte(CP_UTF8, 0, infro_ex.szDevice, -1, device_name,
                            sizeof(device_name), nullptr, nullptr);
    return std::string{device_name, static_cast<size_t>(device_name_size)};
  }
  virtual uint32_t2 GetOffset() const override final {
    MONITORINFO info;
    GetInfo(info);
    return uint32_t2{static_cast<uint32_t>(info.rcMonitor.left),
                     static_cast<uint32_t>(info.rcMonitor.top)};
  }
  virtual bool ResetDisplayMode() override final {
    MONITORINFOEX infro_ex;
    GetInfo(infro_ex);
    auto result = ChangeDisplaySettingsExW(infro_ex.szDevice, nullptr, nullptr,
                                           0, nullptr);
    return (result == DISP_CHANGE_SUCCESSFUL);
  }
  virtual bool SetDisplayMode(
      const DisplayModeDescriptor &display_mode_desc) override final {
    MONITORINFOEX info_ex;
    GetInfo(info_ex);

    DEVMODE dev_mode = {};
    {
      dev_mode.dmSize = sizeof(dev_mode);
      dev_mode.dmFields = (DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY);
      Convert(dev_mode, display_mode_desc);
    }
    auto result = ChangeDisplaySettingsEx(info_ex.szDevice, &dev_mode, nullptr,
                                          CDS_FULLSCREEN, nullptr);

    return (result == DISP_CHANGE_SUCCESSFUL);
  }
  virtual DisplayModeDescriptor GetDisplayMode() const override final {
    /* Get display device name */
    MONITORINFOEX info_ex;
    GetInfo(info_ex);

    /* Get current display settings */
    DEVMODE dev_mode;
    dev_mode.dmSize = sizeof(dev_mode);

    if (EnumDisplaySettings(info_ex.szDevice, ENUM_CURRENT_SETTINGS,
                            &dev_mode) != FALSE) {
      DisplayModeDescriptor display_attributes;
      Convert(display_attributes, dev_mode);
      return display_attributes;
    }

    return {};
  }
  virtual std::vector<DisplayModeDescriptor>
  GetSupportedDisplayModes() const override final {
    std::vector<DisplayModeDescriptor> display_attributes;

    MONITORINFOEX info_ex;
    GetInfo(info_ex);

    const DWORD field_bits =
        (DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY);

    DEVMODE dev_mode;
    dev_mode.dmSize = sizeof(dev_mode);

    for (DWORD mode = 0;
         EnumDisplaySettings(info_ex.szDevice, mode, &dev_mode) != FALSE;
         ++mode) {
      if ((dev_mode.dmFields & field_bits) == field_bits) {
        DisplayModeDescriptor outputDesc;
        Convert(outputDesc, dev_mode);
        display_attributes.push_back(outputDesc);
      }
    }

    FinalizeDisplayModes(display_attributes);

    return display_attributes;
  }
  virtual NativeDisplayHandle GetNativeHandle() const override final {
    return m_monitor;
  }

private:
  HMONITOR m_monitor{nullptr};

  void GetInfo(MONITORINFO &info) const {
    info.cbSize = sizeof(info);
    ::GetMonitorInfo(m_monitor, &info);
  }
  void GetInfo(MONITORINFOEX &info) const {
    info.cbSize = sizeof(info);
    ::GetMonitorInfo(m_monitor, &info);
  }
};

static BOOL CALLBACK Win32MonitorChangedEnumProcedure(HMONITOR monitor, HDC hDC,
                                                      LPRECT rect,
                                                      LPARAM data) {
  auto &info = *reinterpret_cast<MonitorChangedInfo *>(data);
  auto display_iterator =
      std::find_if(g_display_list.begin(), g_display_list.end(),
                   [monitor](const Win32DisplayContainer &entry) -> bool {
                     return (entry.display->GetNativeHandle() == monitor);
                   });
  if (display_iterator != g_display_list.end()) {
    info.registered_monitors++;
  } else {
    info.unregistered_monitors++;
  }
  return TRUE;
}

static BOOL CALLBACK Win32MonitorEnumProcedure(HMONITOR monitor, HDC hDC,
                                               LPRECT rect, LPARAM data) {
  auto monitor_iterator =
      std::find_if(g_display_list.begin(), g_display_list.end(),
                   [monitor](const Win32DisplayContainer &entry) -> bool {
                     return (entry.display->GetNativeHandle() == monitor);
                   });
  if (monitor_iterator != g_display_list.end()) {
    /* Update cache index */
    monitor_iterator->cache_index = g_display_cache_index;
  } else {
    /* Allocate new display object */
    auto display = std::make_unique<Win32Display>(monitor);
    if (display->IsPrimary()) {
      g_primary_display = display.get();
    }
    g_display_list.emplace_back(std::move(display), g_display_cache_index);
  }
  return TRUE;
}

std::span<Display *> Display::GetList() {
  if (UpdateDisplayList()) {
    /* Update reference list and append null terminator to array */
    g_display_ref.clear();
    g_display_ref.reserve(g_display_list.size() + 1);
    for (const auto &entry : g_display_list) {
      g_display_ref.push_back(entry.display.get());
    }
    g_display_ref.push_back(nullptr);
  } else if (g_display_ref.empty()) {
    g_display_ref.clear();
  }
  return std::make_span(reinterpret_cast<Display **>(g_display_ref.data()),
                        g_display_ref.size());
}

Display *Display::Get(size_t index) {
  UpdateDisplayList();
  return (index < g_display_list.size() ? g_display_list[index].display.get()
                                        : nullptr);
}

Display *Display::GetPrimary() {
  UpdateDisplayList();
  return g_primary_display;
}

bool Display::ToggleCursor(bool show) {
  ShowCursor(show ? TRUE : FALSE);
  return true;
}

bool Display::IsCursorToggled() {
  bool visible;
  if (IsCursorVisible(visible)) {
    return visible;
  }
  return false;
}

bool Display::SetCursorPosition(const uint32_t2 &position) {
  return SetCursorPos(position.x, position.y) != FALSE;
}

uint32_t2 Display::GetCursorPosition() {
  POINT point;
  if (GetCursorPos(&point) != FALSE) {
    return {point.x, point.y};
  }
  return {0, 0};
}
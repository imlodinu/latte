#pragma once

#include "fwd.h"
#include "latte/prelude.h"

#include <string>
#include <vector>

namespace latte::platform {
struct DisplayModeDescriptor {
  uint32_t2 resolution;
  uint32_t refresh_rate = 0;
};

class LATTE_API Display : public NonCopyable {
public:
  static std::span<Display *> GetList();
  static Display *Get(size_t index);
  static Display *GetPrimary();
  static bool ToggleCursor(bool show);
  static bool IsCursorToggled();
  static bool SetCursorPosition(const uint32_t2 &position);
  static uint32_t2 GetCursorPosition();

  // methods
  virtual bool IsPrimary() const = 0;
  virtual std::string GetDeviceName() const = 0;
  virtual uint32_t2 GetOffset() const = 0;
  virtual bool ResetDisplayMode() = 0;
  virtual bool
  SetDisplayMode(const DisplayModeDescriptor &display_mode_desc) = 0;
  virtual DisplayModeDescriptor GetDisplayMode() const = 0;
  virtual std::vector<DisplayModeDescriptor>
  GetSupportedDisplayModes() const = 0;
  virtual NativeDisplayHandle GetNativeHandle() const = 0;

protected:
  static void FinalizeDisplayModes(std::vector<DisplayModeDescriptor> &modes);
};
} // namespace latte::platform
#pragma once

#include "fwd.h"
#include "latte/prelude.h"

#include "display.h"

#include <memory>
#include <string>

namespace latte::platform {
struct WindowDescriptor {
  static const uint32_t DEFAULT_WINDOW_X = 800;
  static const uint32_t DEFAULT_WINDOW_Y = 600;

  std::string title = "latte!";
  uint32_t2 position = uint32_t2(0, 0);
  uint32_t2 size = uint32_t2(DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y);
  bool visible = false;
  bool borderless = false;
  bool resizable = false;
  bool accept_drop_files = false;
  const void *window_context = nullptr;
};

class LATTE_API Window : public NonCopyable {
public:
  static std::unique_ptr<Window> Create(const WindowDescriptor &desc);
  virtual ~Window();

  virtual void SetPosition(const uint32_t2 &position) = 0;
  virtual void SetTitle(const std::string &title) = 0;
  virtual void SetShown(bool shown) = 0;
  virtual void SetSize(const uint32_t2 &size, bool use_client_area = true) = 0;
  virtual void SetDescriptor(const WindowDescriptor &desc) = 0;
  virtual void SetFocus() = 0;
  virtual void SetFullscreen(bool fullscreen) = 0;

  virtual uint32_t2 GetPosition() const = 0;
  virtual std::string GetTitle() const = 0;
  virtual bool IsShown() const = 0;
  virtual uint32_t2 GetSize(bool use_client_area = true) const = 0;
  virtual WindowDescriptor GetDescriptor() const = 0;
  virtual bool HasFocus() const = 0;
  virtual bool IsFullscreen() const = 0;

  virtual Display *GetResidentDisplay() const = 0;
  virtual NativeWindowHandle GetNativeHandle() const = 0;

  void PostExit();
  bool HasExit() const;

  bool ProcessEvents();

protected:
  Window();

  virtual void OnProcessEvents() = 0;

private:
  struct Impl;
  Impl *m_impl;
};
} // namespace latte::platform
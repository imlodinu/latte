#include "latte/platform/window.h"

#ifdef LATTE_SYSTEM_WINDOWS
#include "win32/window.inl"
#endif

using namespace latte::platform;

struct Window::Impl {
  bool quit = false;
};

Window::Window() : m_impl(new Impl) {}
Window::~Window() { delete m_impl; }

std::unique_ptr<Window> Window::Create(const WindowDescriptor &desc) {
#ifdef LATTE_SYSTEM_WINDOWS
  return std::make_unique<Win32Window>(desc);
#endif
}

bool Window::ProcessEvents() {
  OnProcessEvents();
  return !HasExit();
}

void Window::PostExit() { m_impl->quit = true; }
bool Window::HasExit() const { return m_impl->quit; }
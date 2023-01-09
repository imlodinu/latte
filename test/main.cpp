#include "latte/latte.h"
#include "latte/platform/window.h"

#include <chrono>
#include <iostream>

using namespace latte;

int main() {
  std::string out_concat = "Hello, world!";
  std::printf("%s", out_concat.c_str());

  auto window_desc = platform::WindowDescriptor{};
  window_desc.title = "LATTE!!!";
  window_desc.resizable = true;
  window_desc.visible = true;

  auto window = platform::Window::Create(window_desc);
  // store current time
  double elapsed_time = 0;
  auto last_time = std::chrono::high_resolution_clock::now();
  while (window->ProcessEvents()) {
    auto current_time = std::chrono::high_resolution_clock::now();
    double delta_time =
        std::chrono::duration<double>(current_time - last_time).count();
    elapsed_time += delta_time;
    last_time = current_time;
    if (elapsed_time > 1.0F) {
      elapsed_time -= 1.0F;
      window->SetFullscreen(!window->IsFullscreen());
    }
  }
}
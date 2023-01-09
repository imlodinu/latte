#include "latte/platform/display.h"

#ifdef LATTE_SYSTEM_WINDOWS
#include "win32/display.inl"
#endif

using namespace latte::platform;

void Display::FinalizeDisplayModes(std::vector<DisplayModeDescriptor> &modes) {
  // Sort modes by resolution
  std::sort(modes.begin(), modes.end(),
            [](const DisplayModeDescriptor &display_desc_a,
               const DisplayModeDescriptor &display_desc_b) {
              return display_desc_a.resolution.x * display_desc_a.resolution.y <
                     display_desc_b.resolution.x * display_desc_b.resolution.y;
            });

  // Remove duplicates
  modes.erase(std::unique(modes.begin(), modes.end(),
                          [](const DisplayModeDescriptor &display_desc_a,
                             const DisplayModeDescriptor &display_desc_b) {
                            auto equality = (display_desc_a.resolution ==
                                             display_desc_b.resolution);
                            return (equality.x != 0U) && (equality.y != 0U);
                          }),
              modes.end());
}
#pragma once

#include "fwd.h"

// Enums are mostly from
// https://github.com/glfw/glfw/blob/master/include/GLFW/glfw3.h

namespace latte::platform {
using key_t = int32_t;
// From line 391
enum class Key : key_t {
  unknown = -1,

  /* Printable keys */
  space = 32,
  apostrophe = 39, /* ' */
  comma = 44,      /* , */
  minus = 45,      /* - */
  period = 46,     /* . */
  slash = 47,      /* / */
  n0 = 48,
  n1 = 49,
  n2 = 50,
  n3 = 51,
  n4 = 52,
  n5 = 53,
  n6 = 54,
  n7 = 55,
  n8 = 56,
  n9 = 57,
  semicolon = 59, /* ; */
  equal = 61,     /* = */
  a = 65,
  b = 66,
  c = 67,
  d = 68,
  e = 69,
  f = 70,
  g = 71,
  h = 72,
  i = 73,
  j = 74,
  k = 75,
  l = 76,
  m = 77,
  n = 78,
  o = 79,
  p = 80,
  q = 81,
  r = 82,
  s = 83,
  t = 84,
  u = 85,
  v = 86,
  w = 87,
  x = 88,
  y = 89,
  z = 90,
  left_bracket = 91,  /* [ */
  backslash = 92,     /* \ */
  right_bracket = 93, /* ] */
  grave_accent = 96,  /* ` */
  world_1 = 161,      /* non-US #1 */
  world_2 = 162,      /* non-US #2 */

  /* Function keys */
  escape = 256,
  enter = 257,
  tab = 258,
  backspace = 259,
  insert = 260,
  del = 261,
  right = 262,
  left = 263,
  down = 264,
  up = 265,
  page_up = 266,
  page_down = 267,
  home = 268,
  end = 269,
  caps_lock = 280,
  scroll_lock = 281,
  num_lock = 282,
  print_screen = 283,
  pause = 284,
  f1 = 290,
  f2 = 291,
  f3 = 292,
  f4 = 293,
  f5 = 294,
  f6 = 295,
  f7 = 296,
  f8 = 297,
  f9 = 298,
  f10 = 299,
  f11 = 300,
  f12 = 301,
  f13 = 302,
  f14 = 303,
  f15 = 304,
  f16 = 305,
  f17 = 306,
  f18 = 307,
  f19 = 308,
  f20 = 309,
  f21 = 310,
  f22 = 311,
  f23 = 312,
  f24 = 313,
  f25 = 314,
  key_pad_0 = 320,
  key_pad_1 = 321,
  key_pad_2 = 322,
  key_pad_3 = 323,
  key_pad_4 = 324,
  key_pad_5 = 325,
  key_pad_6 = 326,
  key_pad_7 = 327,
  key_pad_8 = 328,
  key_pad_9 = 329,
  key_pad_decimal = 330,
  key_pad_divide = 331,
  key_pad_multiply = 332,
  key_pad_subtract = 333,
  key_pad_add = 334,
  key_pad_enter = 335,
  key_pad_equal = 336,
  left_shift = 340,
  left_control = 341,
  left_alt = 342,
  left_super = 343,
  right_shift = 344,
  right_control = 345,
  right_alt = 346,
  right_super = 347,
  menu = 348,

  last = menu,
};

using modifier_key_t = uint32_t;
// From line 533
enum class ModifierKey : modifier_key_t {
  shift = 0x0001,
  control = 0x0002,
  alt = 0x0004,
  super = 0x0008,
  caps_lock = 0x0010,
  num_lock = 0x0020,
};

using mouse_button_t = uint32_t;
// From line 571
enum class MouseButton : mouse_button_t {
  button_1 = 0,
  button_2 = 1,
  button_3 = 2,
  button_4 = 3,
  button_5 = 4,
  button_6 = 5,
  button_7 = 6,
  button_8 = 7,
  last = button_8,
  left = button_1,
  right = button_2,
  middle = button_3,
};

using gamepad_button_t = uint32_t;
// From line 618
enum class GamepadButton : gamepad_button_t {
  a = 0,
  b = 1,
  x = 2,
  y = 3,
  left_bumper = 4,
  right_bumper = 5,
  back = 6,
  start = 7,
  guide = 8,
  left_thumb = 9,
  right_thumb = 10,
  dpad_up = 11,
  dpad_right = 12,
  dpad_down = 13,
  dpad_left = 14,
  last = dpad_left,

  cross = a,
  circle = b,
  square = x,
  triangle = y,
};

using gamepad_axis_t = uint32_t;
// From line 648
enum class GamepadAxis : gamepad_axis_t {
  left_x = 0,
  left_y = 1,
  right_x = 2,
  right_y = 3,
  left_trigger = 4,
  right_trigger = 5,
  last = right_trigger,
};
} // namespace latte::platform
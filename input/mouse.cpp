#include "mouse.h"
#include "../src/log.h"
#include <SDL_mouse.h>
#include <algorithm>
#include <array>
#include <cstdint>
namespace eclipse::input {
int mouse::x = 0;
int mouse::y = 0;
int mouse::xLast = 0;
int mouse::yLast = 0;

std::array<bool, mouse::ButtonCount> mouse::buttons;
std::array<bool, mouse::ButtonCount> mouse::buttonsLast;

void mouse::Inititlaize() {
  std::fill(buttons.begin(), buttons.end(), false);
  std::fill(buttonsLast.begin(), buttonsLast.end(), false);
}

void mouse::Update() {
  uint32_t state = SDL_GetMouseState(&x, &y);

  xLast = x;
  yLast = y;

  buttonsLast = buttons;

  for (int i = 0; i < ButtonCount; i++) {
    buttons[i] = state & SDL_BUTTON(i + 1);
  }
}

bool mouse::Buttons(int button) {
  ECLIPSE_ASSERT(button >= ECLIPSE_INPUT_MOUSE_FIRST &&
                     button <= ECLIPSE_INPUT_MOUSE_LAST,
                 "out of bounds");
  if (button >= ECLIPSE_INPUT_MOUSE_FIRST &&
      button <= ECLIPSE_INPUT_MOUSE_LAST) {
    return buttons[button - 1];
  }
  return false;
}

bool mouse::ButtonsUp(int button) {
  ECLIPSE_ASSERT(button >= ECLIPSE_INPUT_MOUSE_FIRST &&
                     button <= ECLIPSE_INPUT_MOUSE_LAST,
                 "out of bounds");
  if (button >= ECLIPSE_INPUT_MOUSE_FIRST &&
      button <= ECLIPSE_INPUT_MOUSE_LAST) {
    return !buttons[button - 1] && buttonsLast[button - 1];
  }
  return false;
}

bool mouse::ButtonsDown(int button) {
  ECLIPSE_ASSERT(button >= ECLIPSE_INPUT_MOUSE_FIRST &&
                     button <= ECLIPSE_INPUT_MOUSE_LAST,
                 "out of bounds");
  if (button >= ECLIPSE_INPUT_MOUSE_FIRST &&
      button <= ECLIPSE_INPUT_MOUSE_LAST) {
    return buttons[button - 1] && !buttonsLast[button - 1];
  }
  return false;
}
} // namespace eclipse::input

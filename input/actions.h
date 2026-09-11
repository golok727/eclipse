#pragma once

#include "keyboard.h"

namespace eclipse::input {

enum class Action {
  MoveUp,
  MoveDown,
  MoveLeft,
  MoveRight,
  Attack,
  Charge,
  Hurt,
  Die,
  Pause,
};

inline bool ActionDown(Action action) {
  switch (action) {
  case Action::MoveUp:
    return keyboard::Key(ECLIPSE_INPUT_KEY_UP) ||
           keyboard::Key(ECLIPSE_INPUT_KEY_W);
  case Action::MoveDown:
    return keyboard::Key(ECLIPSE_INPUT_KEY_DOWN) ||
           keyboard::Key(ECLIPSE_INPUT_KEY_S);
  case Action::MoveLeft:
    return keyboard::Key(ECLIPSE_INPUT_KEY_LEFT) ||
           keyboard::Key(ECLIPSE_INPUT_KEY_A);
  case Action::MoveRight:
    return keyboard::Key(ECLIPSE_INPUT_KEY_RIGHT) ||
           keyboard::Key(ECLIPSE_INPUT_KEY_D);
  case Action::Attack:
    return keyboard::Key(ECLIPSE_INPUT_KEY_SPACE);
  case Action::Charge:
    return keyboard::Key(ECLIPSE_INPUT_KEY_C);
  case Action::Hurt:
    return keyboard::Key(ECLIPSE_INPUT_KEY_H);
  case Action::Die:
    return keyboard::Key(ECLIPSE_INPUT_KEY_K);
  case Action::Pause:
    return keyboard::Key(ECLIPSE_INPUT_KEY_ESCAPE);
  }
  return false;
}

inline bool ActionPressed(Action action) {
  switch (action) {
  case Action::Attack:
    return keyboard::KeyDown(ECLIPSE_INPUT_KEY_SPACE);
  case Action::Charge:
    return keyboard::KeyDown(ECLIPSE_INPUT_KEY_C);
  case Action::Hurt:
    return keyboard::KeyDown(ECLIPSE_INPUT_KEY_H);
  case Action::Die:
    return keyboard::KeyDown(ECLIPSE_INPUT_KEY_K);
  case Action::Pause:
    return keyboard::KeyDown(ECLIPSE_INPUT_KEY_ESCAPE);
  case Action::MoveUp:
  case Action::MoveDown:
  case Action::MoveLeft:
  case Action::MoveRight:
    return ActionDown(action);
  }
  return false;
}

} // namespace eclipse::input

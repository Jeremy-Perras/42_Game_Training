#pragma once
#include "game_object.hpp"
#include "window.hpp"
namespace ve {
  class MouseMovementController {
  public:
    MouseMovementController(Window &window);
    MouseMovementController(const MouseMovementController &src) = delete;
    MouseMovementController &operator=(const MouseMovementController &rhs) = delete;
    ~MouseMovementController();
    void getCursorPos();
    void getInput(GameObject &gameObject);

  private:
    Window &window_;
    double xpos_;
    double ypos_;
  };

}  // namespace ve
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
    void getInput(GameObject &menuObject, std::vector<std::vector<GameObject>> &playerInterface_);

  private:
    Window &window_;
    double xpos_;
    double ypos_;
    TextureIndex index_ = TextureIndex::WHITE;
    ;
    glm::vec4 color_;
  };

}  // namespace ve
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
    // getter
    void getInput(GameObject &menuObject, std::vector<std::vector<GameObject>> &playerInterface_);
    bool getGameLoop() const { return gameLoop_; }

  private:
    Window &window_;
    double xpos_;
    double ypos_;
    TextureIndex index_ = TextureIndex::WHITE;
    bool gameLoop_ = false;
    glm::vec4 color_{};
  };

}  // namespace ve
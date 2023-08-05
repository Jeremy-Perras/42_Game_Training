#pragma once

#include "game_object.hpp"

namespace ve {
  class MouseMovementController {
  public:
    MouseMovementController(Window &window, GameState &gameState);
    MouseMovementController(const MouseMovementController &src) = delete;
    MouseMovementController &operator=(const MouseMovementController &rhs) = delete;
    ~MouseMovementController();
    void getUserClick(GameObject &menuInterface);
    void changeUserInterface(std::vector<std::vector<GameObject>> &playerInterface_);

    // getter
    void getInput(GameObject &menuInterface,
                  std::vector<std::vector<GameObject>> &playerInterface_);

  private:
    Window &window_;
    GameState &gameState_;

    double xpos_;
    double ypos_;
    TextureIndex index_ = TextureIndex::WHITE;

    glm::vec4 color_{};
  };

}  // namespace ve
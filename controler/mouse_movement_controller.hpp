#pragma once

#include "../gamestructure/game_object.hpp"

namespace ve {
  class MouseMovementController {
  public:
    MouseMovementController(Window &window, GameState &gameState);
    MouseMovementController(const MouseMovementController &src) = delete;
    MouseMovementController &operator=(const MouseMovementController &rhs) = delete;
    ~MouseMovementController();
    void getUserClick(GameObject &menuInterface);
    void getUserClickMenu(GameObject &menuStartInterface);
    void changeUserInterface(std::vector<std::vector<GameObject>> &playerInterface_);

    // getter
    void getInput(GameObject &menuInterface,
                  std::vector<std::vector<GameObject>> &playerInterface_);

  private:
    void coordinatesMouse();
    void setInput();
    Window &window_;
    GameState &gameState_;
    double xpos_;
    double ypos_;
    double xposWindow_;
    double yposWindow_;
    TextureIndex index_ = TextureIndex::WHITE;
    bool mouseSet_ = false;

    glm::vec4 color_{};
  };

}  // namespace ve
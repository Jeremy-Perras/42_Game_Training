#pragma once
#include "choose_level_render_system.hpp"
#include "game_object.hpp"
#include "star_nest_render_system.hpp"
namespace ve {
  class WindowDisplay {
  public:
    WindowDisplay(Device &device, Renderer &renderer, std::vector<GameObject> &menuInterface,
                  std::vector<std::vector<GameObject>> &playerInterface,
                  std::vector<GameObject> &gameInterface, std::vector<GameObject> &displayInterface,
                  std::vector<GameObject> &timeInterface, StarNest &StarNest,
                  ChooseLevel &chooseLevel);
    WindowDisplay(const WindowDisplay &src) = delete;
    WindowDisplay &operator=(const WindowDisplay &rhs) = delete;
    ~WindowDisplay();
    void render(FrameInfo &frameInfo);
    // Getter
  private:
    StarNest &render_system_;
    ChooseLevel &chooseLevel_;
    Device &device_;
    Renderer &renderer_;

    double lastTime_ = 0.0F;
    double lastFrameTime_ = 0.0F;
    std::vector<GameObject> &menuInterface_;
    std::vector<GameObject> &displayInterface_;
    std::vector<std::vector<GameObject>> &playerInterface_;
    std::vector<GameObject> &gameInterface_;
    std::vector<GameObject> &timeInterface_;
  };
}  // namespace ve
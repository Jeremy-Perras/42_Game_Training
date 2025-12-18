#pragma once

#include <chrono>
#include <memory>
#include <vector>

#include "controler/keyboard_movement_controller.hpp"
#include "controler/mouse_movement_controller.hpp"
#include "gamestructure/game_loop.hpp"
#include "gamestructure/game_object.hpp"
#include "renderer/choose_level_render_system.hpp"
#include "renderer/star_nest_render_system.hpp"
#include "renderer/window_display.hpp"
#include "system/device.hpp"
#include "system/renderer.hpp"
#include "system/window.hpp"
#include "vulkan/vulkan_core.h"

namespace ve {

  struct interfaceSize {
    int f0;
    int f1;
    int f2;
  };

  class Application {
  public:
    Application();
    Application(const Application& src) = delete;
    Application& operator=(const Application& rhs) = delete;
    ~Application();
    void mainLoop();

  private:
    VkImageView createImageView(VkImage image, VkFormat format);
    void updateFPS(std::chrono::steady_clock::time_point currentTime);
    void statePlaying();
    void stateLoadingsScreen();
    void stateGameLoop(std::chrono::steady_clock::time_point currentTime);
    static void resetTime(std::chrono::steady_clock::time_point* time);
    void updateFrameInfo();
    void updateGameLvl();
    void StepByStep();
    void updateTimeScreen();
    void menuStart();
    void logicGame();
    void chooseLevel();

    GameState gameState_ = {GameState::MENU};

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    Renderer renderer_{window_, device_};
    MouseMovementController mouse_{window_, gameState_};

    unsigned int fpscount_;
    std::chrono::steady_clock::time_point fpsTime_;
    std::chrono::steady_clock::time_point startGameLoop_;
    std::chrono::steady_clock::time_point passTime_;
    std::chrono::steady_clock::time_point currentTime_;

    bool isAlreadyDone_ = false;

    std::unique_ptr<WindowDisplay> windowDisplay_;
    std::unique_ptr<GameLoop> gameLoop_;
    std::unique_ptr<StarNest> render_system_;
    std::unique_ptr<ChooseLevel> chooseLevel_;

    FrameInfo frameInfo_{};

    std::vector<GameObject> menuInterface_;
    std::vector<GameObject> displayInterface_;
    std::vector<std::vector<GameObject>> playerInterface_;
    std::vector<GameObject> gameInterface_;
    std::vector<GameObject> timeInterface_;
    std::vector<GameObject> menuStartInterface_;
    std::vector<GameObject> exitInterface_;
    std::vector<GameObject> chooseLevelInterface_;
    KeyboardMovementController cameraController_;

    GameObject startLoadingScreen_;

    float timeUpdateGame_ = 0.2;
    int indexLvl = 0;
    int counterTime = 0;
    int play = 0;
  };
}  // namespace ve
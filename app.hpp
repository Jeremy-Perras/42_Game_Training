#pragma once

#include <chrono>
#include <memory>
#include <thread>

#include "compute_shader.hpp"
#include "descriptors.hpp"
#include "device.hpp"
#include "game_loop.hpp"
#include "interface_model.hpp"
#include "mouse_movement_controller.hpp"
#include "renderer.hpp"
#include "vulkan/vulkan_core.h"
#include "window.hpp"

namespace ve {

  struct interfaceSize {
    int f0;
    int f1;
    int f2;
  };

  class Application {
  public:
    Application();
    Application(const Application &src) = delete;
    Application &operator=(const Application &rhs) = delete;
    ~Application();
    void mainLoop();

  private:
    VkImageView createImageView(VkImage image, VkFormat format);
    void updateFPS(std::chrono::steady_clock::time_point currentTime);
    void statePlaying();
    void stateMenu(GameObject &start);
    void stateGameLoop(std::chrono::steady_clock::time_point currentTime);
    void stateStart(std::chrono::steady_clock::time_point currentTime, GameObject &start);
    static void resetTime(std::chrono::steady_clock::time_point *time);
    void updateFrameInfo();
    void updateGameLvl();
    GameState gameState_ = {GameState::PLAYING};

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    Renderer renderer_{window_, device_};
    MouseMovementController mouse_{window_, gameState_};

    unsigned int fpscount_;
    std::chrono::steady_clock::time_point fpsTime_;
    std::chrono::steady_clock::time_point startGameLoop_;
    std::chrono::steady_clock::time_point firstScreenTime_;

    bool isAlreadyDone_ = false;

    std::unique_ptr<GameLoop> gameLoop_;
    std::unique_ptr<ComputeShader> computeShader_;
    FrameInfo frameInfo_{};

    std::vector<GameObject> menuInterface_;
    std::vector<GameObject> displayInterface_;
    std::vector<std::vector<GameObject>> playerInterface_;
    std::vector<GameObject> gameInterface_;

    float timeUpdateGame_ = 0.2;
    int indexLvl = 1;
  };
}  // namespace ve
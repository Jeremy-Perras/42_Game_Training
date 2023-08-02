#pragma once

#include <chrono>

#include "descriptors.hpp"
#include "device.hpp"
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
    void updateFPS(std::chrono::steady_clock::time_point newTime);
    void initDescriptor();
    void gameLoop();

    interfaceSize interfaceSize_{3, 3, 4};
    GameState gameState_ = {GameState::PLAYING};

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    Renderer renderer_{window_, device_};
    MouseMovementController mouse_{window_, gameState_};

    std::unique_ptr<DescriptorPool> textureDescriptorPool_{};
    std::vector<VkDescriptorSet> textureDescriptorSets_;
    std::unique_ptr<DescriptorSetLayout> textureDescriptorSetLayout_;

    std::vector<GameObject> menuInterface_;
    std::vector<std::vector<GameObject>> playerInterface_;
    std::vector<GameObject> gameInterface_;

    unsigned int m_fpscount_;
    std::chrono::steady_clock::time_point startFrameTime_;

    playerCoordinate playerCoordinate_;
    GameObject *playerPointer_;

    std::vector<std::pair<TextureIndex, glm::vec4>> playerInput_;
    int playerIndexInput_ = 0;
  };
}  // namespace ve
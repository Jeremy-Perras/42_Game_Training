#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "descriptors.hpp"
#include "game_object.hpp"
#include "menu.hpp"
#include "model.hpp"
#include "mouse_movement_controller.hpp"
#include "renderer.hpp"
#include "swap_chain.hpp"

namespace ve {
  struct GlobalUbo {
    float deltaTime;
  };

  class Application {
  public:
    Application();
    Application(const Application &src) = delete;
    Application &operator=(const Application &rhs) = delete;
    ~Application();

    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    void mainLoop();

  private:
    VkImageView createImageView(VkImage image, VkFormat format);
    void updateFPS(std::chrono::steady_clock::time_point newTime);
    void createObject();
    void initDescriptor();

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    Renderer renderer_{window_, device_};
    MouseMovementController mouse_{window_};

    std::vector<GameObject> gameObjects_;
    std::unique_ptr<DescriptorPool> globalPool_{};
    std::unique_ptr<DescriptorPool> computePool_{};

    unsigned int m_fpscount_;
    std::vector<std::unique_ptr<Buffer>> uboBuffers_;
    std::unique_ptr<DescriptorSetLayout> descriptorSetLayout_;
    std::vector<VkDescriptorSet> descriptorSets_;

    std::chrono::steady_clock::time_point startFrameTime_;

    std::vector<MenuSystem::Builder> builder_;
  };
}  // namespace ve
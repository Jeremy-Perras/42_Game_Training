#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

#include "descriptors.hpp"
#include "game_object.hpp"
#include "mouse_movement_controller.hpp"
#include "parsing.hpp"
#include "renderer.hpp"
#include "swap_chain.hpp"
#include "texture_render_system.hpp"

namespace ve {

  struct GlobalUbo {
    float deltaTime;
  };
  enum GameState { START, PLAYING, MENU, GAMELOOP };
  struct playerInterfaceCoordinate {
    int ligne = 0;
    int colone[3] = {0, 0, 0};
    void incremetation(int size, int ligne) {
      if (colone[ligne] < size) {
        colone[ligne]++;
      }
    }
  };

  class Application {
  public:
    Application();
    Application(const Application &src) = delete;
    Application &operator=(const Application &rhs) = delete;
    ~Application();
    void mainLoop();
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

  private:
    VkImageView createImageView(VkImage image, VkFormat format);
    void updateFPS(std::chrono::steady_clock::time_point newTime);
    void initDescriptor();
    void gameLoop();

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    Renderer renderer_{window_, device_};
    MouseMovementController mouse_{window_};

    std::unique_ptr<DescriptorPool> globalPool_{};
    std::unique_ptr<DescriptorPool> computePool_{};

    unsigned int m_fpscount_;
    std::vector<std::unique_ptr<Buffer>> uboBuffers_;

    std::vector<VkDescriptorSet> descriptorSets_;

    std::chrono::steady_clock::time_point startFrameTime_;
    std::vector<GameObject> gameObjects_;
    std::vector<std::vector<GameObject>> playerInterface_;
    std::vector<GameObject> gameInterface_;

    std::unique_ptr<DescriptorSetLayout> descriptorSetLayout_;
    std::vector<TextureRenderSystem::Builder> builder_;
    GameState gameState_ = {GameState::PLAYING};

    playerCoordinate playerCoordinate_;
    GameObject *playerPointer_;
    playerInterfaceCoordinate playerStatue_{0, {0, 0, 0}};

    // std::vector<std::pair<class T1, class T2>>
  };
}  // namespace ve
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

  struct interfaceSize {
    int f0;
    int f1;
    int f2;
  };

  struct GlobalUbo {
    float deltaTime;
  };

  enum GameState { START, PLAYING, MENU, GAMELOOP };

  class Application {
  public:
    Application();
    Application(const Application &src) = delete;
    Application &operator=(const Application &rhs) = delete;
    ~Application();
    void mainLoop();

    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    // Getter
    GameState getGameState() const { return gameState_; }
    // Setter
    void setGameState(GameState gameState) { gameState_ = gameState; }

  private:
    VkImageView createImageView(VkImage image, VkFormat format);
    void updateFPS(std::chrono::steady_clock::time_point newTime);
    void initDescriptor();
    void gameLoop();

    interfaceSize interfaceSize_{3, 3, 4};

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    Renderer renderer_{window_, device_};
    MouseMovementController mouse_{window_};

    std::vector<std::unique_ptr<Buffer>> uboBuffers_;
    std::unique_ptr<DescriptorPool> texturePool_{};
    std::vector<VkDescriptorSet> textureDescriptorSets_;
    std::unique_ptr<DescriptorSetLayout> textureDescriptorSetLayout_;

    std::vector<GameObject> gameObjects_;
    std::vector<std::vector<GameObject>> playerInterface_;
    std::vector<GameObject> gameInterface_;

    unsigned int m_fpscount_;
    std::chrono::steady_clock::time_point startFrameTime_;

    GameState gameState_ = {GameState::PLAYING};

    playerCoordinate playerCoordinate_;
    GameObject *playerPointer_;

    std::vector<std::pair<TextureIndex, glm::vec4>> playerInput_;
    int playerIndexInput_ = 0;
  };
}  // namespace ve
#pragma once
#include <_types/_uint32_t.h>
#include <sys/time.h>

#include <stdexcept>

#include "game_object.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"

namespace ve {
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
    void loadGameObjects();
    void createPipelineLayout();
    void createPipeline();
    void createCommandBuffers();
    void freeCommandBuffers();
    void drawFrame();
    static long double getElapsedTime(struct timeval end, struct timeval begin);
    void recreateSwapChain();
    void recordCommandBuffer(int imageIndex);
    void renderGameObjects(VkCommandBuffer commandBuffer);

    Window window_{WIDTH, HEIGHT, "GameEngine"};
    Device device_{window_};
    std::unique_ptr<SwapChain> swapChain_;
    std::unique_ptr<Pipeline> pipeline_;
    VkPipelineLayout pipelineLayout_;
    std::vector<VkCommandBuffer> commandBuffers_;
    std::vector<GameObject> gameObjects_;

    unsigned int m_fpscount_;
    struct timeval start_;
    struct timeval end_;
  };
}  // namespace ve
#include "window_display.hpp"

#include "shader_render_system.hpp"
namespace ve {
  WindowDisplay::WindowDisplay(Device& device, Renderer& renderer,
                               std::vector<GameObject>& menuInterface,
                               std::vector<std::vector<GameObject>>& playerInterface,
                               std::vector<GameObject>& gameInterface,
                               std::vector<GameObject>& displayInterface,
                               std::vector<GameObject>& timeInterface,
                               ShaderRenderSystem& shaderRenderSystem)
      : render_system_(shaderRenderSystem),
        device_(device),
        renderer_(renderer),
        menuInterface_(menuInterface),
        displayInterface_(displayInterface),
        playerInterface_(playerInterface),
        gameInterface_(gameInterface),
        timeInterface_(timeInterface) {}

  WindowDisplay::~WindowDisplay() {}

  void ve::WindowDisplay::render(FrameInfo& frameInfo) {
    auto* commandBuffer = renderer_.beginFrame(false);
    frameInfo.commandBuffer = commandBuffer;
    renderer_.beginSwapChainRenderPass(commandBuffer);

    for (const auto& obj : gameInterface_) {
      if (obj.textureRenderSystem) {
        obj.textureRenderSystem->render(frameInfo);
      }
    }

    for (const auto& obj : displayInterface_) {
      if (obj.textureRenderSystem) {
        obj.textureRenderSystem->render(frameInfo);
      }
    }

    for (const auto& ob : playerInterface_) {
      for (const auto& obj : ob) {
        obj.textureRenderSystem->render(frameInfo);
      }
    }

    for (auto& obj : menuInterface_) {
      obj.textureRenderSystem->render(frameInfo);
    }

    for (auto& obj : timeInterface_) {
      obj.textureRenderSystem->render(frameInfo);
    }

    render_system_.renderGameObjects(frameInfo);

    renderer_.endSwapChainRenderPass(commandBuffer);
    renderer_.endFrame(false);

    double currentTime = glfwGetTime() - frameInfo.Time;
    lastFrameTime_ = (currentTime - lastTime_) * 1000.0;
    lastTime_ = currentTime;
  }
}  // namespace ve
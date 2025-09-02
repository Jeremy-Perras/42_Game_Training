#pragma once

#include "../renderer/exit_render_system.hpp"
#include "../renderer/texture_render_system.hpp"
namespace ve {
  class GameObject {
  public:
    // using id_t = unsigned int;

    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;
    GameObject(GameObject &&) = default;
    GameObject &operator=(GameObject &&) = default;
    ~GameObject() {};

    static GameObject createGameObject() {
      static id_t currentId = 0;
      return GameObject(currentId++);
    }

    std::shared_ptr<TextureRenderSystem> textureRenderSystem;
    std::shared_ptr<ExitRenderSystem> exitRenderSystem;
    // Getter
    id_t getId() const { return id_; }

  private:
    GameObject(id_t objId) : id_{objId} {};

    id_t id_;
  };
}  // namespace ve
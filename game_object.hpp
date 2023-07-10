#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>

#include "model.hpp"
namespace ve {
  struct TransformComponent {
    glm::vec3 translation{};  // (position offset)
    glm::vec3 scale{1.F, 1.F, 1.0F};
    glm::vec3 rotation;

    glm::mat4 mat4() const;
    glm::mat3 normalMatrix();
  };
  class GameObject {
  public:
    using id_t = unsigned int;
    using Map = std::unordered_map<id_t, GameObject>;

    GameObject(const GameObject &) = delete;
    GameObject &operator=(const GameObject &) = delete;
    GameObject(GameObject &&) = default;
    GameObject &operator=(GameObject &&) = default;
    ~GameObject(){};

    static GameObject createGameObject() {
      static id_t currentId = 0;
      return GameObject(currentId++);
    }

    std::shared_ptr<Model> model{};
    glm::vec3 color{};
    TransformComponent transform{};

    // Getter
    id_t getId() const { return id_; }

  private:
    GameObject(id_t objId) : id_{objId} {};

    id_t id_;
  };
}  // namespace ve
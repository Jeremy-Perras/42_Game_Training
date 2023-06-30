#include "keyboard_movement_controller.hpp"

#include <_types/_uint32_t.h>

// std
#include <limits>

namespace ve {

  void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, GameObject& gameObject) {
    (void)gameObject;
    glm::vec2 translation{0};
    if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) {
      translation.y += 0.01F;
    }
    if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) {
      translation.y -= 0.01F;
    }
    if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) {
      translation.x += 0.01F;
    }
    if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) {
      translation.x -= 0.01F;
    }
    // if (gameObject.transform.translation.x + translation.x < 1
    //     && gameObject.transform.translation.x + translation.x > -1) {
    //   gameObject.transform.translation.x += translation.x;
    // }
    // if (gameObject.transform.translation.y + translation.y < 1
    //     && gameObject.transform.translation.y + translation.y > -1) {
    //   gameObject.transform.translation.y += translation.y;
    // }
  }
}  // namespace ve
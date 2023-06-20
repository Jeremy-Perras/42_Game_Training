#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ve {
  class Window {
  public:
    Window();
    ~Window();
    bool shouldClose();
    GLFWwindow* getWindow() { return _window; }
    VkExtent2D getExtent() const {
      return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

  private:
    int width = 800;
    int height = 600;
    void initWindow();

    GLFWwindow* _window;
  };
}  // namespace ve
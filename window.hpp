#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ve {
  class Window {
  public:
    Window();
    ~Window();

    void initWindow();
    bool shouldClose();
    GLFWwindow* getWindow() { return window; }

  private:
    const int WIDTH = 800;
    const int HEIGHT = 600;
    GLFWwindow* window;
  };
}  // namespace ve
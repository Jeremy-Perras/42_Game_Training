#include <_types/_uint32_t.h>

#include <string>

#include "vulkan/vulkan_core.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ve {

  class Window {
  public:
    Window(int w, int h, std::string name);
    ~Window();
    Window(Window const &rhs) = delete;
    Window &operator=(Window const &src) = delete;

    bool shouldClose();
    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);

    GLFWwindow *getWindow() { return window_; }
    VkExtent2D getExtent() const {
      return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)};
    }

  private:
    const int width_;
    const int height_;
    std::string name_;
    GLFWwindow *window_;

    void initWindow();
  };
}  // namespace ve
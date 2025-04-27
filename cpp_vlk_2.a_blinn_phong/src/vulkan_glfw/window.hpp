#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

using std::string;
using std::vector;

class Window {
   public:
    Window(int width, int height, string name);
    ~Window();
    Window(const Window &) = delete;
    Window &operator=(const Window &) = delete;
    bool shouldClose() { return glfwWindowShouldClose(window); }
    bool wasWindowResized() { return framebufferResized; }
    void resetWindowResizedFlag() { framebufferResized = false; }
    GLFWwindow *getGLFWwindow() const { return window; }
    void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
    VkExtent2D getExtent() { return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)}; }

   private:
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
    int width;
    int height;
    string title;
    bool framebufferResized = false;
    GLFWwindow *window;
    void initWindow();
};

#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <fmt/color.h>   // fmt::fg, fmt::bg
#include <fmt/format.h>  // fmt::print

#include <string>
#include <vector>

using fmt::color;
using fmt::print;
using std::string;
using std::vector;

class Window {
   public:
    Window(int width, int height, string title);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    inline int getWidth() { return width; }
    inline int getHeight() { return height; }
    inline string getTitle() { return title; }
    inline GLFWwindow* getWindow() { return window; }
    bool shouldClose() { return glfwWindowShouldClose(window); }
    void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
    VkExtent2D getExtent() {
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

   private:
    int width;
    int height;
    string title;
    GLFWwindow* window;
    void initWindow();
};
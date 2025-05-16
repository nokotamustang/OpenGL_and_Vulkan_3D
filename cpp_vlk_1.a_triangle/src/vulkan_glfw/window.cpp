#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <stdexcept>

#include "window.hpp"

using std::runtime_error;
using std::string;
using std::vector;

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

Window::Window(int width, int height, string title) {
    this->width = width;
    this->height = height;
    this->title = title;
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::initWindow() {
    glfwInit();
    // Don't create an OpenGL context, since we're using Vulkan
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    // Disable window resizing for now
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    // Get the resolution of the primary monitor and center the window
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (mode) {
        const int xPos = (mode->width - width) * 0.5;
        const int yPos = (mode->height - height) * 0.5;
        glfwSetWindowPos(window, xPos, yPos);
    }
    // Set the key callback function for handling keyboard input
    glfwSetKeyCallback(window, key_callback);
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
    // Create the window surface using GLFW
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw runtime_error("failed to create window surface");
    }
}

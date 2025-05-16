#include "window.hpp"

#include <stdexcept>

using std::runtime_error;
using std::string;
using std::vector;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, 1);
    }
}

Window::Window(int w, int h, string name) {
    this->width = w;
    this->height = h;
    this->title = name;
    initWindow();
}

Window::~Window() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

    // Get the resolution of the primary monitor and center the window
    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    if (mode) {
        const int xPos = (mode->width - width) * 0.5;
        const int yPos = (mode->height - height) * 0.5;
        glfwSetWindowPos(window, xPos, yPos);
    }

    glfwSetKeyCallback(window, key_callback);
}

void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface) {
    if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
        throw runtime_error("failed to create window surface");
    }
}

void Window::framebufferResizeCallback(GLFWwindow *glfw_window, int width, int height) {
    auto new_window = reinterpret_cast<Window *>(glfwGetWindowUserPointer(glfw_window));
    new_window->framebufferResized = true;
    new_window->width = width;
    new_window->height = height;
}

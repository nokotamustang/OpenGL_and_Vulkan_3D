#pragma once

#include <fmt/color.h>   // fmt::fg, fmt::bg
#include <fmt/format.h>  // fmt::print

#include <memory>
#include <vector>

#include "vulkan_glfw/descriptors.hpp"
#include "vulkan_glfw/device.hpp"
#include "vulkan_glfw/game_object.hpp"
#include "vulkan_glfw/renderer.hpp"
#include "vulkan_glfw/window.hpp"

using fmt::color;
using fmt::print;

class App {
   public:
    static constexpr int WIDTH = 1600;
    static constexpr int HEIGHT = 900;
    App();
    ~App();
    App(const App &) = delete;
    App &operator=(const App &) = delete;
    void run();

   private:
    void loadGameObjects();
    Window lveWindow{WIDTH, HEIGHT, "Vulkan"};
    LveDevice lveDevice{lveWindow};
    LveRenderer lveRenderer{lveWindow, lveDevice};
    std::unique_ptr<LveDescriptorPool> globalPool{};
    LveGameObject::Map gameObjects;
};

#pragma once

#include <fmt/color.h>   // fmt::fg, fmt::bg
#include <fmt/format.h>  // fmt::print

#include <array>
#include <cassert>
#include <memory>     // std::unique_ptr
#include <stdexcept>  // std::runtime_error
#include <string>     // std::string
#include <vector>     // std::vector

#include "vulkan_glfw/device.hpp"
#include "vulkan_glfw/pipeline.hpp"
#include "vulkan_glfw/swap_chain.hpp"
#include "vulkan_glfw/window.hpp"

using fmt::color;
using fmt::print;
using std::array;
using std::make_unique;
using std::runtime_error;
using std::string;
using std::unique_ptr;
using std::vector;

class App {
   public:
    static const int WIDTH = 1600;
    static const int HEIGHT = 900;
    App();
    ~App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    void run();

   private:
    void new_layout();
    void new_pipeline();
    void new_command_buffers();
    void draw();
    Window window = Window(WIDTH, HEIGHT, "Vulkan");
    Device engineDevice = Device(window);
    PipelineConfig pipelineConfig = Pipeline::defaultPipelineConfig(WIDTH, HEIGHT);
    EngineSwapChain swapChain = EngineSwapChain(engineDevice, window.getExtent());
    unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
    vector<VkCommandBuffer> commandBuffers;
};

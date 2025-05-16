#pragma once

#include <memory>
#include <vector>

#include "vulkan_glfw/camera.hpp"
#include "vulkan_glfw/device.hpp"
#include "vulkan_glfw/frame_info.hpp"
#include "vulkan_glfw/game_object.hpp"
#include "vulkan_glfw/pipeline.hpp"

using std::unique_ptr;

class SimpleRenderSystem {
   public:
    SimpleRenderSystem(
        Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
    ~SimpleRenderSystem();

    SimpleRenderSystem(const SimpleRenderSystem &) = delete;
    SimpleRenderSystem &operator=(const SimpleRenderSystem &) = delete;

    void renderGameObjects(FrameInfo &frameInfo);

   private:
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline(VkRenderPass renderPass);

    Device &device;

    unique_ptr<Pipeline> pipeline;
    VkPipelineLayout pipelineLayout;
};

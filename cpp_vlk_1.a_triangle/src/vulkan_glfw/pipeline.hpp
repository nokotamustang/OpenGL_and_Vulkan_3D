#pragma once

#include <fmt/color.h>   // fmt::fg, fmt::bg
#include <fmt/format.h>  // fmt::print

#include <string>
#include <vector>

#include "device.hpp"

using fmt::color;
using fmt::print;
using std::string;
using std::vector;

struct PipelineConfig {
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    VkPipelineLayout pipelineLayout = nullptr;
    VkRenderPass renderPass = nullptr;
    uint32_t subpass = 0;
};

class Pipeline {
   public:
    Pipeline(Device& device, const PipelineConfig& config, const string& vertFilePath, const string& fragFilePath);
    ~Pipeline();

    Pipeline(const Pipeline&) = delete;        // Delete the copy constructor
    void operator=(const Pipeline&) = delete;  // Delete the assignment operator

    static PipelineConfig defaultPipelineConfig(uint32_t width, uint32_t height);

    void bind(VkCommandBuffer commandBuffer);

   private:
    Device& engineDevice;
    VkPipeline graphicsPipeline;
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;

    static vector<char> readFile(const string& filePath);
    void createGraphicsPipeline(const PipelineConfig& config, const string& vertFilePath, const string& fragFilePath);

    // Pointer to a pointer because the function will be modifying the shader module
    void createShaderModule(const vector<char>& code, VkShaderModule* shaderModule);
};
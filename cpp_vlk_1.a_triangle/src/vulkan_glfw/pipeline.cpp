#include "pipeline.hpp"

#include <cassert>
#include <fstream>
#include <stdexcept>

using std::ifstream;
using std::ios;
using std::numeric_limits;
using std::runtime_error;
using std::string;
using std::vector;

Pipeline::Pipeline(Device& device,
                   const PipelineConfig& config,
                   const string& vertFilePath,
                   const string& fragFilePath) : engineDevice{device} {
    createGraphicsPipeline(config, vertFilePath, fragFilePath);
}

Pipeline::~Pipeline() {
    vkDestroyShaderModule(engineDevice.device(), vertShaderModule, nullptr);
    vkDestroyShaderModule(engineDevice.device(), fragShaderModule, nullptr);
    vkDestroyPipeline(engineDevice.device(), graphicsPipeline, nullptr);
}

vector<char> Pipeline::readFile(const string& filePath) {
    ifstream file(filePath, ios::ate | ios::binary);  // Flags: ate = at the end, binary = read as binary file
    if (!file.is_open()) {
        throw runtime_error("failed to open file");
    }
    size_t fileSize = (size_t)file.tellg();  // Get the size of the file with tellg in bytes
    vector<char> buffer(fileSize);           // Create a buffer with the size of the file
    file.seekg(0);                           // Move the cursor to the beginning of the file
    file.read(buffer.data(), fileSize);      // Read the file into the buffer
    file.close();                            // Close the file
    return buffer;
}

void Pipeline::createGraphicsPipeline(const PipelineConfig& config, const string& vertFilePath, const string& fragFilePath) {
    assert(config.pipelineLayout != nullptr && "cannot create graphics pipeline: no pipelineLayout provided in config");
    assert(config.renderPass != nullptr && "cannot create graphics pipeline: no renderPass provided in config");

    // Read the shader files
    auto vertShaderCode = readFile(vertFilePath);
    auto fragShaderCode = readFile(fragFilePath);
    print("vertex shader {} bytes\n", vertShaderCode.size());
    print("fragment shader {} bytes\n", fragShaderCode.size());
    createShaderModule(vertShaderCode, &vertShaderModule);
    createShaderModule(fragShaderCode, &fragShaderModule);

    // Create the shader stages
    VkPipelineShaderStageCreateInfo shaderStages[2];
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = vertShaderModule;
    shaderStages[0].pName = "main";
    shaderStages[0].flags = 0;
    shaderStages[0].pNext = nullptr;
    shaderStages[0].pSpecializationInfo = nullptr;
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = fragShaderModule;
    shaderStages[1].pName = "main";
    shaderStages[1].flags = 0;
    shaderStages[1].pNext = nullptr;
    shaderStages[1].pSpecializationInfo = nullptr;

    // Vertex input configuration
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;

    // Viewport configuration
    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &config.viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &config.scissor;

    // Create the graphics pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &config.inputAssemblyInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &config.rasterizationInfo;
    pipelineInfo.pMultisampleState = &config.multisampleInfo;
    pipelineInfo.pColorBlendState = &config.colorBlendInfo;
    pipelineInfo.pDepthStencilState = &config.depthStencilInfo;
    pipelineInfo.pDynamicState = nullptr;
    pipelineInfo.layout = config.pipelineLayout;
    pipelineInfo.renderPass = config.renderPass;
    pipelineInfo.subpass = config.subpass;
    pipelineInfo.basePipelineIndex = -1;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    if (vkCreateGraphicsPipelines(
            engineDevice.device(),
            VK_NULL_HANDLE,
            1,
            &pipelineInfo,
            nullptr,
            &graphicsPipeline) != VK_SUCCESS) {
        throw runtime_error("failed to create graphics pipeline");
    }
}

void Pipeline::createShaderModule(const vector<char>& code, VkShaderModule* shaderModule) {
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;     // Type of the create info struct
    createInfo.codeSize = code.size();                                  // Size of the code
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());  // Cast the code data to a uint32_t pointer for Vulkan
    if (vkCreateShaderModule(engineDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
        throw runtime_error("failed to create shader module");
    }
}

PipelineConfig Pipeline::defaultPipelineConfig(uint32_t width, uint32_t height) {
    PipelineConfig config{};

    config.inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    config.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    config.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    config.viewport.x = 0.0f;  // Top left corner of the viewport
    config.viewport.y = 0.0f;
    config.viewport.width = static_cast<float>(width);  // Height and width of the viewport
    config.viewport.height = static_cast<float>(height);
    config.viewport.minDepth = 0.0f;  // Depth of the viewport
    config.viewport.maxDepth = 1.0f;

    config.scissor.offset = {0, 0};  // Offset of the scissor which discards any pixels outside of the scissor
    config.scissor.extent = {width, height};

    // Rasterization configuration
    config.rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    config.rasterizationInfo.depthClampEnable = VK_FALSE;
    config.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    config.rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    config.rasterizationInfo.lineWidth = 1.0f;
    config.rasterizationInfo.cullMode = VK_CULL_MODE_NONE;  // No culling for now, should be VK_CULL_MODE_BACK_BIT
    config.rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    config.rasterizationInfo.depthBiasEnable = VK_FALSE;
    config.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    config.rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    config.rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

    // Multi-sampling configuration
    config.multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    config.multisampleInfo.sampleShadingEnable = VK_FALSE;
    config.multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    config.multisampleInfo.minSampleShading = 1.0f;           // Optional
    config.multisampleInfo.pSampleMask = nullptr;             // Optional
    config.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    config.multisampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    // Color blending configuration
    config.colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    config.colorBlendAttachment.blendEnable = VK_FALSE;
    config.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    config.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    config.colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;              // Optional
    config.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;   // Optional
    config.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;  // Optional
    config.colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;              // Optional

    // Combine the color blending attachment into the color blending configuration
    config.colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    config.colorBlendInfo.logicOpEnable = VK_FALSE;
    config.colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
    config.colorBlendInfo.attachmentCount = 1;
    config.colorBlendInfo.pAttachments = &config.colorBlendAttachment;
    config.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
    config.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
    config.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
    config.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

    // Depth and stencil testing configuration
    config.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    config.depthStencilInfo.depthTestEnable = VK_TRUE;
    config.depthStencilInfo.depthWriteEnable = VK_TRUE;
    config.depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    config.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    config.depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    config.depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    config.depthStencilInfo.stencilTestEnable = VK_FALSE;
    config.depthStencilInfo.front = {};  // Optional
    config.depthStencilInfo.back = {};   // Optional

    return config;
}

void Pipeline::bind(VkCommandBuffer commandBuffer) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);  // Bind as a graphics pipeline
}

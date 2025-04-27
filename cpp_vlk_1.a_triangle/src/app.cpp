#include "app.hpp"

App::App() {
    string build_type = "Unknown";
#ifdef COMPILE_DEBUG
    build_type = "Debug";
#elif COMPILE_RELEASE
    build_type = "Release";
#elif COMPILE_DEVELOPMENT
    build_type = "Development";
#endif
    print("build type: {}\n", build_type);

    new_layout();
    new_pipeline();
    new_command_buffers();
}

App::~App() {
    vkDestroyPipelineLayout(engineDevice.device(), pipelineLayout, nullptr);
}

void App::run() {
    while (!window.shouldClose()) {
        glfwPollEvents();
        draw();
    }
    vkDeviceWaitIdle(engineDevice.device());
}

void App::new_command_buffers() {
    commandBuffers.resize(swapChain.imageCount());  // Resize the command buffer vector to the number of swap chain images

    VkCommandBufferAllocateInfo allocInfo = {};  // Command buffer allocation info struct
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = engineDevice.getCommandPool();                        // Command pool
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;                            // Command buffer level
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());  // Number of command buffers

    if (vkAllocateCommandBuffers(engineDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw runtime_error("failed to allocate command buffers");
    }

    for (int i = 0; i < commandBuffers.size(); ++i) {
        VkCommandBufferBeginInfo beginInfo = {};  // Command buffer begin info struct
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
            throw runtime_error("failed to begin recording command buffer");
        }

        VkRenderPassBeginInfo renderPassInfo = {};  // Render pass begin info struct
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain.getRenderPass();              // Render pass
        renderPassInfo.framebuffer = swapChain.getFrameBuffer(i);           // Frame buffer
        renderPassInfo.renderArea.offset = {0, 0};                          // Offset
        renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();  // Extent

        array<VkClearValue, 2> clearValues = {};          // Array of clear values
        clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};  // Clear color
        clearValues[1].depthStencil = {1.0f, 0};          // Clear depth and stencil

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());  // Number of clear values
        renderPassInfo.pClearValues = clearValues.data();                            // Pointer to clear values

        vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);  // Begin render pass recording
        pipeline->bind(commandBuffers[i]);                                                     // Bind the pipeline

        vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);  // Draw the triangle

        vkCmdEndRenderPass(commandBuffers[i]);  // End render pass recording
        if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
            throw runtime_error("failed to record command buffer");
        }
    }
}

void App::draw() {
    uint32_t imageIndex;
    auto result = swapChain.acquireNextImage(&imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw runtime_error("failed to acquire next image");
    }
    result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
    if (result != VK_SUCCESS) {
        throw runtime_error("failed to present image");
    }
}

void App::new_layout() {
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;             // Optional
    pipelineLayoutInfo.pSetLayouts = nullptr;          // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0;     // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr;  // Optional
    if (vkCreatePipelineLayout(engineDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        print(stderr, fg(color::red), "failed to create pipeline layout\n");
        throw runtime_error("");
    }
}

void App::new_pipeline() {
    // Use swap chain dimensions since they might differ from the window
    auto pipelineConfig = Pipeline::defaultPipelineConfig(swapChain.width(), swapChain.height());
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipelineConfig.renderPass = swapChain.getRenderPass();
    pipeline = make_unique<Pipeline>(engineDevice, pipelineConfig,
                                     "../../../shaders/compiled/simple.vert.spv",
                                     "../../../shaders/compiled/simple.frag.spv");
}

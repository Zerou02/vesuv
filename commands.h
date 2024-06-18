#ifndef commands_h
#define commands_h

#include "common.cpp"

VkCommandPool createCommandPool(QueueFamilyIndices queueIndices, VkDevice logicalDevice);
VkCommandBuffer beginSingleTimeCommands(VkDevice logicalDevice, VkCommandPool commandPool);
void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueues queues, VkDevice logicalDevice, VkCommandPool commandPool);
std::vector<VkCommandBuffer> createCommandBuffers(int size, VkCommandPool pool, VkDevice device);
void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, GraphicsPipeline graphicsPipeline, SwapChain swapchain, VkRenderPass renderPass, std::vector<Uniforms> uniforms, int currentFrame, std::vector<Buffer> vertexBuffer, std::vector<Buffer> indexBuffer);

#endif
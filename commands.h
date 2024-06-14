#ifndef commands_h
#define commands_h

#include "common.cpp"

VkCommandPool createCommandPool(QueueFamilyIndices queueIndices, VkDevice logicalDevice);
VkCommandBuffer beginSingleTimeCommands(VkDevice logicalDevice, VkCommandPool commandPool);
void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueues queues, VkDevice logicalDevice, VkCommandPool commandPool);
std::vector<VkCommandBuffer> createCommandBuffers(int size, VkCommandPool pool, VkDevice device);
void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex, GraphicsPipeline graphicsPipeline, SwapChain swapchain, VkRenderPass renderPass, Buffer vertexBuffer, Buffer indexBuffer, std::vector<VkDescriptorSet> descriptorSets, std::vector<uint16_t> quadIndices, int currentFrame, Buffer vertexBuffer2);

#endif
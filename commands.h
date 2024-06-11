#ifndef commands_h
#define commands_h

#include "common.cpp"

VkCommandPool createCommandPool(QueueFamilyIndices queueIndices, VkDevice logicalDevice);
VkCommandBuffer beginSingleTimeCommands(VkDevice logicalDevice, VkCommandPool commandPool);
void endSingleTimeCommands(VkCommandBuffer commandBuffer, VkQueues queues, VkDevice logicalDevice, VkCommandPool commandPool);

#endif
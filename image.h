#ifndef image_h
#define image_h

#include "common.cpp"

VkImageView createImageView(VkImage image, VkFormat format, VkDevice logicalDevice);
Texture createTextureImage(VkDevice logicalDevice, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueues queues);

#endif
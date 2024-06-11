#ifndef vkMemory_h
#define vkMemory_h

#include "common.cpp"

Buffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDevice logicalDevice, VkPhysicalDevice physical);
uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice);
#endif
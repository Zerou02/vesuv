#ifndef device_h
#define device_h

#include "common.cpp"

bool checkValidationLayerSupport(std::vector<const char *> layers);
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR &surface);
VkPhysicalDevice pickPhysicalDevice(VkInstance &instance, VkSurfaceKHR &surface);
QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR &surface);
VkQueues getQueues(VkDevice logicalDevice, QueueFamilyIndices indices);
VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
QueueFamilyIndices getIndices(VkPhysicalDevice device, VkSurfaceKHR surface);
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR &surface);

#endif
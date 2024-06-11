#ifndef swapChain_h
#define swapChain_h

#include "common.cpp"

SwapChain createSwapChain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow *window);
void createFramebuffers(SwapChain &swapchain, VkRenderPass renderPass, VkDevice logicalDevice);

#endif
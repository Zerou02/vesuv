#ifndef swapChain_h
#define swapChain_h

#include "common.cpp"

SwapChain createSwapChain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow *window);
void createFramebuffers(SwapChain &swapchain, VkRenderPass renderPass, VkDevice logicalDevice);
void cleanupSwapChain(SwapChain &swapChain, VkDevice logicalDevice);
void recreateSwapChain(GLFWwindow *window, VkDevice logicalDevice, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, SwapChain &swapChain, VkRenderPass renderPass);

#endif
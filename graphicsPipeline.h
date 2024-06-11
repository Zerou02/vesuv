#ifndef graphics_pipeline_h
#define graphics_pipeline_h

#include "common.cpp"

VkRenderPass createRenderPass(SwapChain swapchain, VkDevice logicalDevice);
VkDescriptorSetLayout createDescriptorSetLayout(VkDevice logicalDevice);
GraphicsPipeline createGraphicsPipeline(std::string shaderName, VkDevice logicalDevice, SwapChain swapchain, VkDescriptorSetLayout descriptorLayout, VkRenderPass renderPass);
#endif
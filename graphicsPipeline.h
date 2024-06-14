#ifndef graphics_pipeline_h
#define graphics_pipeline_h

#include "common.cpp"

VkRenderPass createRenderPass(SwapChain swapchain, VkDevice logicalDevice);
VkDescriptorSetLayout createDescriptorSetLayout(VkDevice logicalDevice);
GraphicsPipeline createGraphicsPipeline(std::string shaderName, VkDevice logicalDevice, SwapChain swapchain, VkDescriptorSetLayout descriptorLayout, VkRenderPass renderPass);
VkDescriptorPool createDescriptorPool(int size, VkDevice logicalDevice);
std::vector<VkDescriptorSet> createDescriptorSets(int size, VkDescriptorSetLayout layout, VkDescriptorPool pool, VkDevice logicalDevice, VkImageView view, std::vector<Buffer> uniformBuffers, VkSampler sampler);
SyncObjects createSyncObjects(int amount, VkDevice logicalDevice);

#endif
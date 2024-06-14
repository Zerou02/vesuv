#ifndef vesuv_h
#define vesuv_h

#include "common.cpp"

class Vesuv
{
public:
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkInstance instance;
    SwapChain swapChain;
    Window window;
    QueueFamilyIndices queueIndices;
    VkQueues queues;
    VkRenderPass renderPass;
    SyncObjects syncObjects;
    VkCommandPool commandPool;
    VkDescriptorPool descriptorPool;
    int MAX_FRAMES_IN_FLIGHT = 2;

    Vesuv();
    void cleanup();
    VkDescriptorSetLayout createUniformLayouts(std::vector<VkDescriptorType> types, int amountInVertexShader);
    GraphicsPipeline createGraphicPipeline(VkDescriptorSetLayout layout, std::string shaderName);
    Texture createTexture(std::string name);
};
#endif
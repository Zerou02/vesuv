#ifndef vesuv_h
#define vesuv_h

#include "common.cpp"
#include "vertex.h"

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
    std::vector<VkCommandBuffer> commandBuffers;
    int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;
    bool framebufferResized = false;

    Vesuv();
    void cleanup();
    VkDescriptorSetLayout createUniformLayouts(std::vector<VkDescriptorType> types, int amountInVertexShader);
    GraphicsPipeline createGraphicPipeline(VkDescriptorSetLayout layout, std::string shaderName);
    Texture createTexture(std::string name);
    Uniforms createUniforms(std::vector<VkDescriptorType> types, int amountInVertexShader, Texture texture, VkSampler sampler);
    std::vector<Buffer> createUniformBuffers(int amount);
    Buffer createVBO(std::vector<Vertex> vertices);
    Buffer createIndexBuffer(std::vector<uint16_t> indices);
    void drawFrame(std::vector<Buffer> vertices, std::vector<Buffer> indices, GraphicsPipeline graphicsPipeline);
    void destroySampler(VkSampler sampler);
    void destroyTexture(Texture texture);
    void destroyPipeline(GraphicsPipeline pipeline);
    void destroyUniforms(Uniforms uniforms);
    void destroyBuffer(Buffer buffer);
    void listExtensionProperties();
};

#endif
#include "common.cpp"
#include "window.h"
#include "instance.h"
#include "device.h"
#include "swapchain.h"
#include "graphicsPipeline.h"
#include "commands.h"
#include "vesuv.h"
#include "image.h"
#include "vkMemory.h"

Vesuv::Vesuv()
    : physicalDevice{},
      logicalDevice{},
      instance{},
      swapChain{},
      window{},
      queueIndices{},
      queues{},
      renderPass{},
      syncObjects{},
      commandPool{},
      descriptorPool{}, MAX_FRAMES_IN_FLIGHT{2}
{
    std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

    this->window.window = createWindow();
    this->instance = createInstance(validationLayers);
    this->window.surface = createSurface(this->window.window, this->instance);
    this->physicalDevice = pickPhysicalDevice(this->instance, this->window.surface);
    this->logicalDevice = createLogicalDevice(this->physicalDevice, this->window.surface);
    this->queueIndices = findQueueFamilies(this->physicalDevice, this->window.surface);
    this->queues = getQueues(this->logicalDevice, this->queueIndices);
    this->swapChain = createSwapChain(physicalDevice, logicalDevice, this->window.surface, this->window.window);
    this->renderPass = createRenderPass(swapChain, logicalDevice);
    createFramebuffers(swapChain, renderPass, logicalDevice);
    this->commandPool = createCommandPool(queueIndices, logicalDevice);
    this->descriptorPool = createDescriptorPool(MAX_FRAMES_IN_FLIGHT, logicalDevice);
    this->syncObjects = createSyncObjects(MAX_FRAMES_IN_FLIGHT, logicalDevice);
    this->commandBuffers = createCommandBuffers(MAX_FRAMES_IN_FLIGHT, commandPool, logicalDevice);
};

void Vesuv::cleanup()
{
    cleanupSwapChain(swapChain, logicalDevice);
    vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        vkDestroySemaphore(logicalDevice, syncObjects.renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(logicalDevice, syncObjects.imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(logicalDevice, syncObjects.inFlightFences[i], nullptr);
    }
    vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
    vkDestroySurfaceKHR(instance, window.surface, nullptr);
    vkDestroyDevice(logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
    vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

    glfwDestroyWindow(window.window);
}

VkDescriptorSetLayout Vesuv::createUniformLayouts(std::vector<VkDescriptorType> types, int amountInVertexShader)
{
    return createDescriptorSetLayout(logicalDevice, types, amountInVertexShader);
}

GraphicsPipeline Vesuv::createGraphicPipeline(VkDescriptorSetLayout layout, std::string shaderName)
{
    return createGraphicsPipeline(shaderName, logicalDevice, swapChain, layout, renderPass);
}

Texture Vesuv::createTexture(std::string name)
{
    Texture texture;
    texture = createTextureImage(logicalDevice, physicalDevice, commandPool, queues, name);
    texture.imageView = createTextureImageView(texture, logicalDevice);
    return texture;
}

std::vector<Buffer> Vesuv::createUniformBuffers(int amount)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    std::vector<Buffer> uniformBuffers(amount);

    for (size_t i = 0; i < amount; i++)
    {
        uniformBuffers[i] = createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicalDevice, physicalDevice);
        vkMapMemory(logicalDevice, uniformBuffers[i].bufferMemory, 0, bufferSize, 0, &uniformBuffers[i].memMap);
    }
    return uniformBuffers;
}

Uniforms Vesuv::createUniforms(std::vector<VkDescriptorType> types, int amountInVertexShader, Texture texture, VkSampler sampler)
{
    Uniforms uniforms;
    uniforms.descriptorSetLayout = createUniformLayouts(types, amountInVertexShader);
    uniforms.uniformBuffers = createUniformBuffers(MAX_FRAMES_IN_FLIGHT);
    uniforms.descriptorSets = createDescriptorSets(MAX_FRAMES_IN_FLIGHT, uniforms.descriptorSetLayout, descriptorPool, logicalDevice, texture.imageView, uniforms.uniformBuffers, sampler);
    return uniforms;
}

//{
/* public:
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

    int
    void cleanup()
    {
        cleanupSwapChain(swapChain, logicalDevice);
        vkDestroyDescriptorPool(logicalDevice, descriptorPool, nullptr);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroySemaphore(logicalDevice, syncObjects.renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(logicalDevice, syncObjects.imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(logicalDevice, syncObjects.inFlightFences[i], nullptr);
        }
        vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
        vkDestroySurfaceKHR(instance, window.surface, nullptr);
        vkDestroyDevice(logicalDevice, nullptr);
        vkDestroyInstance(instance, nullptr);
        vkDestroyRenderPass(logicalDevice, renderPass, nullptr);

        glfwDestroyWindow(window.window);
    }
    Vesuv()
    {
        std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

        this->window.window = createWindow();
        this->instance = createInstance(validationLayers);
        this->window.surface = createSurface(this->window.window, this->instance);
        this->physicalDevice = pickPhysicalDevice(this->instance, this->window.surface);
        this->logicalDevice = createLogicalDevice(this->physicalDevice, this->window.surface);
        this->queueIndices = findQueueFamilies(this->physicalDevice, this->window.surface);
        this->queues = getQueues(this->logicalDevice, this->queueIndices);
        this->swapChain = createSwapChain(physicalDevice, logicalDevice, this->window.surface, this->window.window);
        this->renderPass = createRenderPass(swapChain, logicalDevice);
        createFramebuffers(swapChain, renderPass, logicalDevice);
        this->commandPool = createCommandPool(queueIndices, logicalDevice);

        this->syncObjects = createSyncObjects(MAX_FRAMES_IN_FLIGHT, logicalDevice);
    }
}; */
#include "common.cpp"
#include "window.h"
#include "instance.h"
#include "device.h"
#include "swapchain.h"
#include "graphicsPipeline.h"
#include "commands.h"
#include "vesuv.h"

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
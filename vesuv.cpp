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
#include "vertex.h"

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
      descriptorPool{},
      MAX_FRAMES_IN_FLIGHT{2},
      currentFrame{0},
      framebufferResized{false}
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
    vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
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

    glfwDestroyWindow(window.window);
    glfwTerminate();
}

void Vesuv::destroySampler(VkSampler sampler)
{
    vkDestroySampler(logicalDevice, sampler, nullptr);
}

void Vesuv::destroyTexture(Texture texture)
{
    vkDestroyImageView(logicalDevice, texture.imageView, nullptr);
    vkDestroyImage(logicalDevice, texture.textureImage, nullptr);
    vkFreeMemory(logicalDevice, texture.textureImageMemory, nullptr);
}

void Vesuv::destroyPipeline(GraphicsPipeline pipeline)
{
    vkDestroyPipeline(logicalDevice, pipeline.pipeline, nullptr);
    vkDestroyPipelineLayout(logicalDevice, pipeline.layout, nullptr);
    vkDestroyShaderModule(logicalDevice, pipeline.fragShader, nullptr);
    vkDestroyShaderModule(logicalDevice, pipeline.vertShader, nullptr);
}

void Vesuv::destroyUniforms(Uniforms uniforms)
{
    vkDestroyDescriptorSetLayout(logicalDevice, uniforms.descriptorSetLayout, nullptr);
    for (size_t i = 0; i < uniforms.amountSetElements; i++)
    {
        vkDestroyBuffer(logicalDevice, uniforms.uniformBuffers[i].buffer, nullptr);
        vkFreeMemory(logicalDevice, uniforms.uniformBuffers[i].bufferMemory, nullptr);
    }
}

void Vesuv::destroyBuffer(Buffer buffer)
{
    vkDestroyBuffer(logicalDevice, buffer.buffer, nullptr);
    vkFreeMemory(logicalDevice, buffer.bufferMemory, nullptr);
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
    uniforms.amountSetElements = types.size();
    uniforms.descriptorSetLayout = createUniformLayouts(types, amountInVertexShader);
    uniforms.uniformBuffers = createUniformBuffers(MAX_FRAMES_IN_FLIGHT);
    uniforms.descriptorSets = createDescriptorSets(MAX_FRAMES_IN_FLIGHT, uniforms.descriptorSetLayout, descriptorPool, logicalDevice, texture.imageView, uniforms.uniformBuffers, sampler);
    return uniforms;
}

Buffer Vesuv::createVBO(std::vector<Vertex> vertices)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    auto stagingBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicalDevice, physicalDevice);

    void *data;
    vkMapMemory(logicalDevice, stagingBuffer.bufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, stagingBuffer.bufferMemory);

    auto vertexBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, logicalDevice, physicalDevice);
    vertexBuffer.amountElements = vertices.size();
    copyBuffer(stagingBuffer.buffer, vertexBuffer.buffer, bufferSize, logicalDevice, commandPool, queues);
    vkDestroyBuffer(logicalDevice, stagingBuffer.buffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBuffer.bufferMemory, nullptr);
    return vertexBuffer;
}

Buffer Vesuv::createIndexBuffer(std::vector<uint16_t> indices)
{
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    auto stagingBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, logicalDevice, physicalDevice);

    void *data;
    vkMapMemory(logicalDevice, stagingBuffer.bufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(logicalDevice, stagingBuffer.bufferMemory);

    auto indexBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, logicalDevice, physicalDevice);
    indexBuffer.amountElements = indices.size();
    copyBuffer(stagingBuffer.buffer, indexBuffer.buffer, bufferSize, logicalDevice, commandPool, queues);

    vkDestroyBuffer(logicalDevice, stagingBuffer.buffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBuffer.bufferMemory, nullptr);
    return indexBuffer;
}

void Vesuv::drawFrame(std::vector<Buffer> vertices, std::vector<Buffer> indices, GraphicsPipeline graphicsPipeline)
{
    vkWaitForFences(logicalDevice, 1, &syncObjects.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    auto result = vkAcquireNextImageKHR(logicalDevice, swapChain.swapchain, UINT64_MAX, syncObjects.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapChain(window.window, logicalDevice, window.surface, physicalDevice, swapChain, renderPass);
        return;
    }
    else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        throw std::runtime_error("failed to acquire swapchain image");
    }
    vkResetFences(logicalDevice, 1, &syncObjects.inFlightFences[currentFrame]);

    vkResetCommandBuffer(commandBuffers[currentFrame], 0);
    recordCommandBuffer(commandBuffers[currentFrame], imageIndex, graphicsPipeline, swapChain, renderPass, graphicsPipeline.uniforms, currentFrame, vertices, indices);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {syncObjects.imageAvailableSemaphores[currentFrame]};

    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {syncObjects.renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    if (vkQueueSubmit(queues.graphicsQueue, 1, &submitInfo, syncObjects.inFlightFences[currentFrame]) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {swapChain.swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;
    result = vkQueuePresentKHR(queues.presentationQueue, &presentInfo);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
        framebufferResized = false;
        recreateSwapChain(window.window, logicalDevice, window.surface, physicalDevice, swapChain, renderPass);
    }
    else if (result != VK_SUCCESS)
    {
        throw std::runtime_error("failed to present swap chain image!");
    }

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Vesuv::listExtensionProperties()
{
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    for (int i = 0; i < extensionCount; i++)
    {
        printf("%s\n", extensions[i].extensionName);
    }
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
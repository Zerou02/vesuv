#include "common.cpp"
#include "window.h"
#include "instance.h"
#include "device.h"
#include "swapchain.h"
#include "image.h"
#include "graphicsPipeline.h"
#include "commands.h"
#include "vkMemory.h"
#include "vesuv.h"
#include "vertex.h"

class Main
{
public:
    Vesuv vesuv;
    GraphicsPipeline graphicsPipeline;
    std::vector<VkCommandBuffer> commandBuffers;
    const int MAX_FRAMES_IN_FLIGHT = 2;
    uint32_t currentFrame = 0;
    Buffer vertexBuffer;
    Buffer quadBuffer;
    Buffer indexBuffer;
    Buffer VBO2;
    VkDescriptorSetLayout descriptorSetLayout;
    bool framebufferResized = false;
    std::vector<Buffer> uniformBuffers;
    std::vector<VkDescriptorSet> descriptorSets;
    Texture texture;

    VkImageView textureImageView;
    VkSampler textureSampler;

    /*
        const std::vector<Vertex> vertices = {
            {{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}}; */

    /*     const std::vector<Vertex> vertices = {
            // tri
            {{-1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // oben links
            {{1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // oben rechts
            {{0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // unten rechts
            // pos,col?,tex
            {{-1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // oben links
            {{0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // oben rechts
            {{0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // unten rechts
            {{0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // unten rechts
            {{-1.0f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // unten links
            {{-1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // oben links

        }; */
    const std::vector<Vertex> vertices = {
        // tri
        {{-1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // oben links
        {{1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // oben rechts
        {{0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // unten rechts
    };

    const std::vector<uint16_t>
        quadIndices = {0, 1, 2, 2, 3, 0};

    const std::vector<Vertex> triVertices = {
        // pos,col?,tex
        {{-1.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}}, // oben links
        {{1.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},  // oben rechts
        {{0.0f, 1.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // unten rechts
    };

    void listExtensionProperties()
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
    void cleanup()
    {
        vesuv.cleanup();
        vkDestroySampler(vesuv.logicalDevice, textureSampler, nullptr);
        vkDestroyImageView(vesuv.logicalDevice, textureImageView, nullptr);
        vkDestroyImage(vesuv.logicalDevice, texture.textureImage, nullptr);
        vkFreeMemory(vesuv.logicalDevice, texture.textureImageMemory, nullptr);
        vkDestroyPipeline(vesuv.logicalDevice, graphicsPipeline.pipeline, nullptr);
        vkDestroyPipelineLayout(vesuv.logicalDevice, graphicsPipeline.layout, nullptr);
        vkDestroyShaderModule(vesuv.logicalDevice, graphicsPipeline.fragShader, nullptr);
        vkDestroyShaderModule(vesuv.logicalDevice, graphicsPipeline.vertShader, nullptr);
        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
        {
            vkDestroyBuffer(vesuv.logicalDevice, uniformBuffers[i].buffer, nullptr);
            vkFreeMemory(vesuv.logicalDevice, uniformBuffers[i].bufferMemory, nullptr);
        }
        vkDestroyDescriptorSetLayout(vesuv.logicalDevice, descriptorSetLayout, nullptr);
        vkDestroyBuffer(vesuv.logicalDevice, quadBuffer.buffer, nullptr);
        vkFreeMemory(vesuv.logicalDevice, quadBuffer.bufferMemory, nullptr);
        vkDestroyBuffer(vesuv.logicalDevice, vertexBuffer.buffer, nullptr);
        vkFreeMemory(vesuv.logicalDevice, vertexBuffer.bufferMemory, nullptr);
        vkDestroyBuffer(vesuv.logicalDevice, indexBuffer.buffer, nullptr);
        vkFreeMemory(vesuv.logicalDevice, indexBuffer.bufferMemory, nullptr);

        glfwTerminate();
    }

    void updateUniformBuffer(uint32_t currentImage)
    {
        static auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
        UniformBufferObject ubo{};

        ubo.model = glm::mat4(1.0f);
        // ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        //   ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
        ubo.view = glm::mat4(1.0f);
        ubo.proj = glm::mat4(1.0f);
        // ubo.proj = glm::ortho(0, 800, 600, 0);
        //  ubo.proj = glm::perspective(glm::radians(45.0f), swapChain.extent.width / (float)swapChain.extent.height, 0.1f, 10.0f);
        ubo.proj[1][1] *= -1; // openGL hat anderes Koordinatensystem
        memcpy(uniformBuffers[currentImage].memMap, &ubo, sizeof(ubo));
    }

    void drawFrame()
    {
        vkWaitForFences(vesuv.logicalDevice, 1, &vesuv.syncObjects.inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        auto result = vkAcquireNextImageKHR(vesuv.logicalDevice, vesuv.swapChain.swapchain, UINT64_MAX, vesuv.syncObjects.imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            recreateSwapChain(vesuv.window.window, vesuv.logicalDevice, vesuv.window.surface, vesuv.physicalDevice, vesuv.swapChain, vesuv.renderPass);
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            throw std::runtime_error("failed to acquire swapchain image");
        }
        vkResetFences(vesuv.logicalDevice, 1, &vesuv.syncObjects.inFlightFences[currentFrame]);

        vkResetCommandBuffer(commandBuffers[currentFrame], 0);
        recordCommandBuffer(commandBuffers[currentFrame], imageIndex, graphicsPipeline, vesuv.swapChain, vesuv.renderPass, vertexBuffer, indexBuffer, descriptorSets, quadIndices, currentFrame, VBO2);
        updateUniformBuffer(currentFrame);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore waitSemaphores[] = {vesuv.syncObjects.imageAvailableSemaphores[currentFrame]};

        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

        VkSemaphore signalSemaphores[] = {vesuv.syncObjects.renderFinishedSemaphores[currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        if (vkQueueSubmit(vesuv.queues.graphicsQueue, 1, &submitInfo, vesuv.syncObjects.inFlightFences[currentFrame]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = {vesuv.swapChain.swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;
        result = vkQueuePresentKHR(vesuv.queues.presentationQueue, &presentInfo);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
        {
            framebufferResized = false;
            recreateSwapChain(vesuv.window.window, vesuv.logicalDevice, vesuv.window.surface, vesuv.physicalDevice, vesuv.swapChain, vesuv.renderPass);
        }
        else if (result != VK_SUCCESS)
        {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    Buffer createVBO(std::vector<Vertex> vertices)
    {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        auto stagingBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vesuv.logicalDevice, vesuv.physicalDevice);

        void *data;
        vkMapMemory(vesuv.logicalDevice, stagingBuffer.bufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(vesuv.logicalDevice, stagingBuffer.bufferMemory);

        auto vertexBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vesuv.logicalDevice, vesuv.physicalDevice);
        copyBuffer(stagingBuffer.buffer, vertexBuffer.buffer, bufferSize, vesuv.logicalDevice, vesuv.commandPool, vesuv.queues);
        vkDestroyBuffer(vesuv.logicalDevice, stagingBuffer.buffer, nullptr);
        vkFreeMemory(vesuv.logicalDevice, stagingBuffer.bufferMemory, nullptr);
        return vertexBuffer;
    }

    Buffer createIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(quadIndices[0]) * quadIndices.size();

        auto stagingBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vesuv.logicalDevice, vesuv.physicalDevice);

        void *data;
        vkMapMemory(vesuv.logicalDevice, stagingBuffer.bufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, quadIndices.data(), (size_t)bufferSize);
        vkUnmapMemory(vesuv.logicalDevice, stagingBuffer.bufferMemory);

        auto indexBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vesuv.logicalDevice, vesuv.physicalDevice);

        copyBuffer(stagingBuffer.buffer, indexBuffer.buffer, bufferSize, vesuv.logicalDevice, vesuv.commandPool, vesuv.queues);

        vkDestroyBuffer(vesuv.logicalDevice, stagingBuffer.buffer, nullptr);
        vkFreeMemory(vesuv.logicalDevice, stagingBuffer.bufferMemory, nullptr);
        return indexBuffer;
    }

    std::vector<Buffer> createUniformBuffers(int amount)
    {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        std::vector<Buffer> uniformBuffers(amount);

        for (size_t i = 0; i < amount; i++)
        {
            uniformBuffers[i] = createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vesuv.logicalDevice, vesuv.physicalDevice);
            vkMapMemory(vesuv.logicalDevice, uniformBuffers[i].bufferMemory, 0, bufferSize, 0, &uniformBuffers[i].memMap);
        }
        return uniformBuffers;
    }

    Main()
    {
        this->vesuv = Vesuv();

        // this->descriptorSetLayout = createDescriptorSetLayout(vesuv.logicalDevice);
        this->descriptorSetLayout = vesuv.createUniformLayouts(std::vector<VkDescriptorType>{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER}, 1);
        this->graphicsPipeline = createGraphicsPipeline("tri", vesuv.logicalDevice, vesuv.swapChain, descriptorSetLayout, vesuv.renderPass);
        this->texture = createTextureImage(vesuv.logicalDevice, vesuv.physicalDevice, vesuv.commandPool, vesuv.queues);
        this->textureImageView = createTextureImageView(texture, vesuv.logicalDevice);
        this->textureSampler = createTextureSampler(vesuv.physicalDevice, vesuv.logicalDevice);
        this->vertexBuffer = createVBO(vertices);
        this->VBO2 = createVBO(triVertices);
        this->indexBuffer = createIndexBuffer();
        this->uniformBuffers = createUniformBuffers(MAX_FRAMES_IN_FLIGHT);
        this->descriptorSets = createDescriptorSets(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout, vesuv.descriptorPool, vesuv.logicalDevice, textureImageView, uniformBuffers, textureSampler);
        this->commandBuffers = createCommandBuffers(MAX_FRAMES_IN_FLIGHT, vesuv.commandPool, vesuv.logicalDevice);

        auto last = glfwGetTime();
        double elapsed = 0;
        auto frameCount = 0;

        while (!glfwWindowShouldClose(vesuv.window.window))
        {
            auto curr = glfwGetTime();
            elapsed += curr - last;
            last = curr;
            frameCount++;

            if (elapsed >= 1.0f)
            {
                printf("drawn %d frames in 1 second\n", frameCount);
                elapsed = 0;
                frameCount = 0;
            }
            drawFrame();

            if (glfwGetKey(this->vesuv.window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(this->vesuv.window.window, true);
            }

            glfwPollEvents();
        }
        vkDeviceWaitIdle(vesuv.logicalDevice);
        cleanup();
    }
};

int main()
{
    Main main;
}

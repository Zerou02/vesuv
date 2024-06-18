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
#include "vertexData.h"

class Main
{
public:
    Vesuv vesuv;
    GraphicsPipeline graphicsPipeline;
    Buffer vertexBuffer;
    Buffer indexBuffer;
    Buffer VBO2;
    Texture texture;
    VkSampler textureSampler;

    void updateUniformBuffer(Uniforms &uniforms, uint32_t currentImage)
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
        // ubo.proj[1][1] *= -1; // openGL hat anderes Koordinatensystem
        memcpy(uniforms.uniformBuffers[currentImage].memMap, &ubo, sizeof(ubo));
    }

    Main()
    {
        this->texture = vesuv.createTexture("statue");
        this->textureSampler = createTextureSampler(vesuv.physicalDevice, vesuv.logicalDevice);

        auto uniforms = vesuv.createUniforms(std::vector<VkDescriptorType>{
                                                 VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                                                 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                                             },
                                             1, texture, textureSampler);
        this->graphicsPipeline = vesuv.createGraphicPipeline(uniforms.descriptorSetLayout, "tri");
        graphicsPipeline.uniforms = std::vector<Uniforms>{uniforms, uniforms};
        this->vertexBuffer = vesuv.createVBO(quadVertices);
        this->VBO2 = vesuv.createVBO(triVertices);
        this->indexBuffer = vesuv.createIndexBuffer(quadIndices);

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
            vesuv.drawFrame(std::vector<Buffer>{vertexBuffer, VBO2}, std::vector<Buffer>{indexBuffer, indexBuffer}, graphicsPipeline);
            updateUniformBuffer(graphicsPipeline.uniforms[0], vesuv.currentFrame);
            updateUniformBuffer(graphicsPipeline.uniforms[1], vesuv.currentFrame);

            if (glfwGetKey(this->vesuv.window.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(this->vesuv.window.window, true);
            }

            glfwPollEvents();
        }
        vkDeviceWaitIdle(vesuv.logicalDevice);
        vesuv.destroyTexture(texture);
        vesuv.destroySampler(textureSampler);
        vesuv.destroyPipeline(graphicsPipeline);
        vesuv.destroyUniforms(uniforms);
        vesuv.destroyBuffer(VBO2);
        vesuv.destroyBuffer(indexBuffer);
        vesuv.destroyBuffer(vertexBuffer);

        vesuv.cleanup();
    }
};

int main()
{
    Main main;
}

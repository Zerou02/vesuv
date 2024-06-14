#ifndef types_cpp
#define types_cpp

#include "common.cpp"
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentableFamily;

    bool isComplete()
    {
        return graphicsFamily.has_value() && presentableFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct VkQueues
{
    VkQueue graphicsQueue;
    VkQueue presentationQueue;
};

struct SwapChain
{
    VkSwapchainKHR swapchain;
    VkExtent2D extent;
    VkFormat imageFormat;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<VkFramebuffer> framebuffers;
};

struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

struct GraphicsPipeline
{
    VkPipeline pipeline;
    VkPipelineLayout layout;
    VkShaderModule vertShader;
    VkShaderModule fragShader;
};

struct Buffer
{
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
    void *memMap;
};

struct Texture
{
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
};

struct SyncObjects
{
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
};

struct Window
{
    GLFWwindow *window;
    VkSurfaceKHR surface;
};
#endif
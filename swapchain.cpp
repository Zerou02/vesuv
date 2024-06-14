#include "common.cpp"
#include "device.h"
#include "image.h"

VkSurfaceFormatKHR chooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    auto retFormat = availableFormats[0];
    for (auto x : availableFormats)
    {
        if (x.format == VK_FORMAT_B8G8R8_SRGB && x.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            retFormat = x;
            break;
        }
    }
    return retFormat;
}

VkPresentModeKHR chooseSwapPresentationMode(std::vector<VkPresentModeKHR> &availableModes)
{
    VkPresentModeKHR retMode = VK_PRESENT_MODE_FIFO_KHR;
    for (auto x : availableModes)
    {
        if (x == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            retMode = VK_PRESENT_MODE_MAILBOX_KHR;
            break;
        }
    }
    return retMode;
}

// windowSize
VkExtent2D chooseSwapExtent(VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window)
{
    VkExtent2D retVal;
    // magic value: use window width
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        retVal = capabilities.currentExtent;
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        retVal = actualExtent;
    }
    return retVal;
}

void createImageViews(SwapChain &swapChain, VkDevice &logicalDevice)
{
    swapChain.imageViews.resize(swapChain.images.size());

    for (uint32_t i = 0; i < swapChain.images.size(); i++)
    {
        swapChain.imageViews[i] = createImageView(swapChain.images[i], swapChain.imageFormat, logicalDevice);
    }
}
SwapChain createSwapChain(VkPhysicalDevice physicalDevice, VkDevice logicalDevice, VkSurfaceKHR surface, GLFWwindow *window)
{
    auto swapChainSupport = querySwapChainSupport(physicalDevice, surface);
    auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    auto presentationMode = chooseSwapPresentationMode(swapChainSupport.presentModes);
    auto extent = chooseSwapExtent(swapChainSupport.capabilities, window);
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    // magicVal: 0 = noMaximum
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR info{};
    info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface = surface;
    info.minImageCount = imageCount;
    info.imageFormat = surfaceFormat.format;
    info.imageColorSpace = surfaceFormat.colorSpace;
    info.imageExtent = extent;
    info.imageArrayLayers = 1;
    info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice, surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentableFamily.value()};
    if (indices.graphicsFamily != indices.presentableFamily)
    {
        info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = 2;
        info.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        info.queueFamilyIndexCount = 0;
        info.pQueueFamilyIndices = nullptr;
    }
    info.preTransform = swapChainSupport.capabilities.currentTransform;
    info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode = presentationMode;
    info.clipped = VK_TRUE;
    info.oldSwapchain = nullptr;

    SwapChain swapChain;
    if (vkCreateSwapchainKHR(logicalDevice, &info, nullptr, &swapChain.swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain");
    }
    vkGetSwapchainImagesKHR(logicalDevice, swapChain.swapchain, &imageCount, nullptr);
    swapChain.images.resize(imageCount);
    vkGetSwapchainImagesKHR(logicalDevice, swapChain.swapchain, &imageCount, swapChain.images.data());

    swapChain.imageFormat = surfaceFormat.format;
    swapChain.extent = extent;
    createImageViews(swapChain, logicalDevice);
    return swapChain;
}

void createFramebuffers(SwapChain &swapchain, VkRenderPass renderPass, VkDevice logicalDevice)
{
    swapchain.framebuffers.resize(swapchain.images.size());
    for (int i = 0; i < swapchain.framebuffers.size(); i++)
    {
        VkImageView attachments[] = {swapchain.imageViews[i]};
        VkFramebufferCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        info.renderPass = renderPass;
        info.attachmentCount = 1;
        info.pAttachments = attachments;
        info.width = swapchain.extent.width;
        info.height = swapchain.extent.height;
        info.layers = 1;
        if (vkCreateFramebuffer(logicalDevice, &info, nullptr, &swapchain.framebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void cleanupSwapChain(SwapChain &swapChain, VkDevice logicalDevice)
{
    for (size_t i = 0; i < swapChain.framebuffers.size(); i++)
    {
        vkDestroyFramebuffer(logicalDevice, swapChain.framebuffers[i], nullptr);
    }
    for (size_t i = 0; i < swapChain.imageViews.size(); i++)
    {
        vkDestroyImageView(logicalDevice, swapChain.imageViews[i], nullptr);
    }
    vkDestroySwapchainKHR(logicalDevice, swapChain.swapchain, nullptr);
}
void recreateSwapChain(GLFWwindow *window, VkDevice logicalDevice, VkSurfaceKHR surface, VkPhysicalDevice physicalDevice, SwapChain &swapChain, VkRenderPass renderPass)
{
    int width = 0, height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(logicalDevice);
    cleanupSwapChain(swapChain, logicalDevice);

    createSwapChain(physicalDevice, logicalDevice, surface, window);
    createFramebuffers(swapChain, renderPass, logicalDevice);
}
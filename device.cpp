#include "common.cpp"

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR &surface)
{
    QueueFamilyIndices indices;
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> families(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, families.data());
    int i = 0;
    for (auto x : families)
    {
        if (x.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport)
        {
            indices.presentableFamily = i;
        }
        i++;
    }
    return indices;
}

bool checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    uint32_t extCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, availableExtensions.data());
    auto allAvailable = true;
    for (auto x : deviceExtensions)
    {
        auto available = false;
        for (auto y : availableExtensions)
        {
            if (strcmp(x, y.extensionName))
            {
                available = true;
                break;
            }
        }
        if (!available)
        {
            allAvailable = false;
            break;
        }
    }
    return allAvailable;
}

SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR &surface)
{
    SwapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    details.formats.resize(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    details.presentModes.resize(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &formatCount, details.presentModes.data());

    return details;
}

bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR &surface)
{
    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    auto swapChainSupport = querySwapChainSupport(device, surface);
    bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

VkPhysicalDevice pickPhysicalDevice(VkInstance &instance, VkSurfaceKHR &surface)
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
    {
        throw std::runtime_error("No devices found");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    VkPhysicalDevice retDevice = nullptr;
    for (auto x : devices)
    {
        if (!isDeviceSuitable(x, surface))
        {
            continue;
        }
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(x, &properties);
        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(x, &features);
        if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            printf("selected: %s\n", properties.deviceName);
            retDevice = x;
        }
    }
    return retDevice;
}

QueueFamilyIndices getIndices(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    return findQueueFamilies(device, surface);
}

VkDevice createLogicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
{
    std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    auto indices = getIndices(physicalDevice, surface);
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentableFamily.value()};
    auto queuePrio = 1.0f;
    for (auto x : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = x;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePrio;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    info.pQueueCreateInfos = queueCreateInfos.data();
    info.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    info.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    info.ppEnabledExtensionNames = deviceExtensions.data();
    info.pEnabledFeatures = &deviceFeatures;
    VkDevice logicalDevice;
    vkCreateDevice(physicalDevice, &info, nullptr, &logicalDevice);
    return logicalDevice;
}

VkQueues getQueues(VkDevice logicalDevice, QueueFamilyIndices indices)
{
    VkQueues queues;
    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &queues.graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentableFamily.value(), 0, &queues.presentationQueue);
    return queues;
}
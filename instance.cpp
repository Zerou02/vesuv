#include "common.cpp"

bool checkValidationLayerSupport(std::vector<const char *> layers)
{
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    auto retVal = true;
    for (auto layerName : layers)
    {
        auto layerFound = false;
        for (auto availables : availableLayers)
        {
            if (!strcmp(layerName, availables.layerName))
            {
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
        {
            retVal = false;
            break;
        }
    }
    return retVal;
}

VkInstance createInstance(std::vector<const char *> layers)
{
    VkInstance instance;
    if (!checkValidationLayerSupport(layers))
    {
        printf("Error: Not all layers available");
        return instance;
    };
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = layers.size();
    createInfo.ppEnabledLayerNames = layers.data();
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
    return instance;
}
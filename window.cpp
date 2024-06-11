#include "./common.cpp"

/* static void framebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    // auto app = reinterpret_cast<Vesuv *>(glfwGetWindowUserPointer(window));
    // app->framebufferResized = true;
}
 */

GLFWwindow *createWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    auto window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
    return window;
    // glfwSetWindowUserPointer(window, this);
    // glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
};
VkSurfaceKHR createSurface(GLFWwindow *window, VkInstance &instance)
{
    VkSurfaceKHR surface;
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}
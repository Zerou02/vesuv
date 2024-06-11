#ifndef window_h
#define window_h

#include "./common.cpp"

GLFWwindow *createWindow();

VkSurfaceKHR createSurface(GLFWwindow *window, VkInstance &instance);

#endif
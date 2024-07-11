#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

namespace Engine {
class Window{
public:
    Window();
    ~Window();
    void init();
    void update();
    void destroy();
    inline GLFWwindow* getWindow(){return window;}
    inline bool is_windowClosd(){return glfwWindowShouldClose(window);}
private:
    GLFWwindow* window;
};
}

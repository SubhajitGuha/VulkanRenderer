//
//  window.cpp
//  vulkan_learning
//
//  Created by Subhajit on 16/06/24.
//

#include "window.hpp"

namespace Engine {
Window::Window(){
    init();
}

Window::~Window(){
    destroy();
}

void Window::init()
{
    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    window = glfwCreateWindow(640, 480, "Vulkan Window", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return;
    }
}

void Window::update()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
}

void Window::destroy()
{
    glfwTerminate();
    glfwDestroyWindow(window);
}

}

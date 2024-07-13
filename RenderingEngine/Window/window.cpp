//
//  window.cpp
//  vulkan_learning
//
//  Created by Subhajit on 16/06/24.
//

#include "window.hpp"
#include"../Events/KeyEvent.h"
#include"../Events/ApplicationEvent.h"
#include"../Events/MouseEvent.h"

namespace Engine {
Window::Window(const std::string& window_name, uint32_t width, uint32_t height){
    init(window_name, width, height);
}

Window::Window(){
    init("Default Window", 512, 512);
}

Window::~Window(){
    destroy();
}

void Window::init(std::string window_name, uint32_t width, uint32_t height)
{
    m_Data.width = width;
    m_Data.height = height;
    m_Data.name = window_name;
    
    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    m_window = glfwCreateWindow(width, height, window_name.c_str(), NULL, NULL);
    if (!m_window)
    {
        glfwTerminate();
        return;
    }
    glfwSetWindowUserPointer(m_window, &m_Data);

    //set GLFW callbacks
            glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.width = width;
                data.height = height;
                WindowResizeEvent Eventresize(width, height);
                data.CallbackFunc(Eventresize);//this calls the OnEvent () func in application class
                });


            glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int mods) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                switch (action) {
                case GLFW_PRESS:
                {
                    MouseButtonPressed EventPressed(button);
                    data.CallbackFunc(EventPressed);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleased EventReleased(button);
                    data.CallbackFunc(EventReleased);
                    break;
                }
                }

                });

            glfwSetScrollCallback(m_window, [](GLFWwindow* window, double xoffset, double yoffset) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                MouseScrollEvent EventScroll(xoffset, yoffset);
                data.CallbackFunc(EventScroll);
                });

            glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                MouseMoveEvent EventCursorPos(xpos, ypos);
                data.CallbackFunc(EventCursorPos);
                });

            glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

                switch (action) {
                case GLFW_PRESS:
                {
                    KeyPressedEvent EventKeyPressed(key, 0);
                    data.CallbackFunc(EventKeyPressed);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent EventKeyReleased(key);
                    data.CallbackFunc(EventKeyReleased);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent EventKeyRepeat(key, 1);
                    data.CallbackFunc(EventKeyRepeat);
                    break;
                }
                }
                });

            glfwSetCharCallback(m_window, [](GLFWwindow* window, unsigned int codepoint) {
                WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
                KeyTypedEvent Typed(codepoint);
                Data.CallbackFunc(Typed);
                });

            glfwSetWindowCloseCallback(m_window, [](GLFWwindow* window) {
                WindowData& Data = *(WindowData*)glfwGetWindowUserPointer(window);
                WindowCloseEvent EventWindClose;
                Data.CallbackFunc(EventWindClose);
                });
}

void Window::update()
{
    /* Swap front and back buffers */
    glfwSwapBuffers(m_window);

    /* Poll for and process events */
    glfwPollEvents();
}

void Window::destroy()
{
    glfwTerminate();
    glfwDestroyWindow(m_window);
}

}

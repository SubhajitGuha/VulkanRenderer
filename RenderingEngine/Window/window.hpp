#pragma once
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>
#include "../Events/Event.h"
#include <functional>
#include <string>

namespace Engine {
class Window{
public:
    using EventCallbackFunc = std::function<void(Event&)>;
    Window(const std::string& window_name, uint32_t width, uint32_t height);
    Window();
    ~Window();
    void init(std::string window_name, uint32_t width, uint32_t height);
    void update();
    void destroy();
    inline uint32_t GetWidth(){ return m_Data.width;}
    inline uint32_t GetHeight() {return m_Data.height;}
    inline GLFWwindow* getWindow(){return m_window;}
    void SetCallbackEvent(const EventCallbackFunc& callback) { m_Data.CallbackFunc = callback; }

    struct WindowData{
        uint32_t width, height;
        std::string name;
        EventCallbackFunc CallbackFunc;
    };
private:
    GLFWwindow* m_window;
    WindowData m_Data;
};
}

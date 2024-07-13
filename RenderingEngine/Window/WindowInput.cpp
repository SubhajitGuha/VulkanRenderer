#include "WindowInput.hpp"
#include "../application.hpp"


namespace Engine {
    Input* Input::m_Input = new WindowInput();

    bool WindowInput::IsKeyPressedImpl(int keyCode)
    {
        
        int status = glfwGetKey((GLFWwindow*)Application::Get()->GetWindow()->getWindow(), keyCode);
        return status==GLFW_PRESS || status==GLFW_REPEAT;
    }

    bool WindowInput::IsMouseButtonPressed(int Button)
    {
        
        int status = glfwGetMouseButton((GLFWwindow*)Application::Get()->GetWindow()->getWindow(), Button);
        return status == GLFW_PRESS;
    }

    std::pair<double, double> WindowInput::GetMousePos()
    {
        std::pair<double, double> coordinate;
        glfwGetCursorPos((GLFWwindow*) Application::Get()->GetWindow()->getWindow(),&coordinate.first,&coordinate.second);
        return coordinate;
    }

}

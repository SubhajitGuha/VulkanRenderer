#include "SceneCamera.hpp"

namespace Engine {
    SceneCamera::SceneCamera(float Width, float Height)
    {
        SetViewportSize(Width/Height);
    }
    SceneCamera::SceneCamera()
    {
        RecalculateProjection();
        RecalculateProjectionView();
    }
    void SceneCamera::SetPerspctive(float v_FOV, float Near, float Far)
    {
        m_verticalFOV = v_FOV;
        m_PerspectiveNear = Near;
        m_PerspectiveFar = Far;

        RecalculateProjection();
        RecalculateProjectionView();
    }
    void SceneCamera::SetCameraPosition(const glm::vec3& pos)
    {
        m_Position = pos; RecalculateProjectionView();
    }
    void SceneCamera::SetViewDirection(const glm::vec3& dir)
    {
        m_ViewDirection = dir; RecalculateProjectionView();
    }

    void SceneCamera::SetUPVector(const glm::vec3& up)
    {
        Up = up; RecalculateProjectionView();
    }
    void SceneCamera::SetViewportSize(float aspectratio)
    {
        m_AspectRatio = aspectratio;
        RecalculateProjection();
        RecalculateProjectionView();
    }
    glm::mat4 SceneCamera::GetProjectionView()
    {
        return m_ProjectionView;
    }
    inline glm::mat4 SceneCamera::GetViewMatrix()
    {
        return m_View;
    }
    inline glm::mat4 SceneCamera::GetProjectionMatrix()
    {
        return m_Projection;
    }
    inline glm::vec3 SceneCamera::GetCameraPosition()
    {
        return m_Position;
    }
    inline glm::vec3 SceneCamera::GetViewDirection()
    {
        return m_ViewDirection;
    }
    inline float SceneCamera::GetAspectRatio()
    {
        return m_AspectRatio;
    }
    inline float SceneCamera::GetVerticalFOV()
    {
        return m_verticalFOV;
    }
//    void SceneCamera::OnEvent(Event& e)
//    {
//
//    }

    void SceneCamera::OnUpdate()
    {
        float deltatime = 1.0f;
        m_movespeed = 20;
                RightVector  = glm::cross(m_ViewDirection, Up);//we get the right vector (as it is always perpendicular to up and m_ViewDirection)
                
                if (Input::IsKeyPressed(VKR_KEY_W))
                    m_Position += m_ViewDirection * glm::vec3(m_movespeed*deltatime);//move along the View direction
                if (Input::IsKeyPressed(VKR_KEY_S))
                    m_Position -= m_ViewDirection * glm::vec3(m_movespeed*deltatime);//move along the View direction
                if (Input::IsKeyPressed(VKR_KEY_A))
                    m_Position -= RightVector * glm::vec3(m_movespeed*deltatime);//move along the right vector
                if (Input::IsKeyPressed(VKR_KEY_D))
                    m_Position += RightVector * glm::vec3(m_movespeed*deltatime);
                if(Input::IsKeyPressed(VKR_KEY_Q))
                    m_Position += Up * glm::vec3(m_movespeed*deltatime);//move along up vector
                if (Input::IsKeyPressed(VKR_KEY_E))
                    m_Position -= Up * glm::vec3(m_movespeed*deltatime);
                //if (Input::IsKeyPressed(HZ_KEY_R))//reset camera when R is pressed
                //{
                //    m_Position = { 0,0,-1 };
                //    m_ViewDirection = { 0,0,1 };
                //    m_verticalFOV = 45.0f;
                //}

                glm::vec2 NewMousePos = { Input::GetCursorPosition().first,Input::GetCursorPosition().second };
                if (Input::IsButtonPressed(VKR_MOUSE_BUTTON_2))//camera pan
                {
                    auto delta = NewMousePos - OldMousePos;//get change in mouse position

                    m_ViewDirection = glm::mat3(glm::rotate(glm::radians(-delta.x) * 0.1f, Up)) * m_ViewDirection;//invert it
                    m_ViewDirection = glm::mat3(glm::rotate(glm::radians(-delta.y) * 0.1f, RightVector)) * m_ViewDirection;//rotate along right vector
                    m_ViewDirection = glm::normalize(m_ViewDirection);
                }
                OldMousePos = NewMousePos;

                RecalculateProjectionView();
    }

    void SceneCamera::RotateCamera(float pitch , float yaw, float roll)
    {
        m_pitch = pitch;
        m_yaw = yaw;
        m_roll = roll;
        //pitch = glm::clamp(pitch, -89.0f, 89.0f);
        m_ViewDirection = glm::mat3(glm::rotate(glm::radians(yaw), Up)) * glm::mat3(glm::rotate(glm::radians(pitch), RightVector)) * glm::mat3(glm::rotate(glm::radians(roll), m_ViewDirection)) * glm::vec3(0, 0, 1);
        RecalculateProjectionView();
    }

    void SceneCamera::SetOrthographic(float Size, float Near, float Far)
    {
        m_projection_type = ProjectionTypes::Orhtographic;
        m_OrthographicSize = Size;
        m_OrthographicNear = Near;
        m_OrthographicFar = Far;
        RecalculateProjection();
    }

    void SceneCamera::RecalculateProjection()
    {
        switch (m_projection_type)
        {
        case ProjectionTypes::Orhtographic:
        {
            float left = -m_AspectRatio * m_OrthographicSize * 0.5;
            float right = m_AspectRatio * m_OrthographicSize * 0.5;
            float bottom = -m_OrthographicSize * 0.5;
            float Up = m_OrthographicSize * 0.5;
            m_Projection = glm::ortho(left, right, bottom, Up, m_OrthographicNear, m_OrthographicFar);
            return;
        }
        case ProjectionTypes::perspective:
            m_Projection = glm::perspective(glm::radians(m_verticalFOV), m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
            return;
        }
    }
    void SceneCamera::RecalculateProjectionView()
    {
        //moving the camera is nothing but transforming the world
        m_View = glm::lookAt(m_Position, glm::normalize(m_ViewDirection) + m_Position, Up);//this gives the view matrix
        m_ProjectionView = m_Projection * m_View;
    }
}

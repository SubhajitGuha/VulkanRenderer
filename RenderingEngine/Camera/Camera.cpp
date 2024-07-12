#include "Camera.hpp"
//#include "EditorCamera.h"
#include "SceneCamera.hpp"

namespace Engine {

    std::shared_ptr<Camera> Camera::GetCamera(CameraType type)
    {
        switch (type)
        {
//        case CameraType::EDITOR_CAMERA:
//            return std::make_shared<EditorCamera>();
        case CameraType::SCENE_CAMERA:
            return std::make_shared<SceneCamera>();
        default:
            std::cerr<<("Select a valid Camera Type\n");
            return nullptr;
        }
    }
}

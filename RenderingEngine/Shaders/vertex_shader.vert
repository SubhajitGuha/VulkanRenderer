
#version 450 core
layout (location = 0)in vec4 pos;
layout (location = 1)in vec4 color;


layout (location = 0) out vec4 frag_color;
layout (set = 0, binding = 0) uniform CameraBuffer{
    mat4 viewProj;
}cameraData;

void main()
{
    gl_Position = cameraData.viewProj * pos;
    frag_color = color;
}

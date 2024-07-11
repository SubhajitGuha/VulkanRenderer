
#version 450 core
layout (location = 0)in vec4 pos;
layout (location = 1)in vec4 color;


layout (location = 0) out vec4 frag_color;
void main()
{
    gl_Position = pos;
    frag_color = color;
}

#version 450 core

layout (location = 0) out vec4 color;

layout (location = 0) in vec4 frag_color;
void main(){
    color = frag_color;
}

#version 330 core
in vec4 colorV;
layout (location = 0) out vec3 color;

void main()
{    
    color = vec3(colorV);
}
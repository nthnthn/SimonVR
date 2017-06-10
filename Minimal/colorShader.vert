#version 330 core
layout (location = 0) in vec3 position;
out vec4 colorV;

uniform mat4 projection;
uniform mat4 modelview;
uniform vec4 color;

void main()
{
    gl_Position =   projection * modelview * vec4(position, 1.0);  
    colorV = color;
}
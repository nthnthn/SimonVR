#version 330 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D texture_diffuse1;

void main()
{    
    color = vec4(0.5f, 0.5f*TexCoords.y, 0.1f*TexCoords.x, 0.0f);
}